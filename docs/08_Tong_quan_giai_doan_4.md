# TỔNG QUAN GIAI ĐOẠN 4

**Mục tiêu:** chạy mô phỏng trọn một ngày (nhận bệnh phút 0–479, sau đó khám hết người còn lại) và in, ghi lại trạng thái từng phút. Từ kết quả, tính thời gian chờ theo mức ưu tiên và so sánh 4 kịch bản trên cùng một bộ dữ liệu: 1 hoặc 2 phòng/khoa, bật hoặc tắt ưu tiên. Kết quả cuối là bảng so sánh có nhận xét, dùng thẳng cho báo cáo.

## 1. Hiện trạng đầu giai đoạn

Giai đoạn 3 đã merge vào `develop` ngày 23/11 (PR #17, #18, #19, #20). Trên macOS, `make test-all` đạt và `make test-console` đạt 17/17 ca.

| Phần | Đã có | Còn thiếu cho giai đoạn 4 |
|---|---|---|
| Mô phỏng (Leader) | Bước phút: hoàn tất → tiếp nhận → gọi khám; tiếp nhận xếp phòng ngay; chạy được quá giờ đóng nhận đến khi hết người | Chưa có kiểm tra tự động các bất biến trên một ngày đông người |
| Nhật ký (C) | Menu 5 và menu 6 ("in từng phút") in snapshot ra màn hình | Chưa ghi được ra tệp; snapshot không nói phút đó ai vừa xong, ai vừa được gọi |
| Thống kê (B) | Chờ TB/max theo mức, hàng chờ lớn nhất theo phòng và toàn hệ thống, danh sách chờ Merge Sort, `saveRunResults` | Chưa có chờ trung bình chung toàn hệ thống và phút kết thúc ngày |
| So sánh (menu 9) | Chạy 4 kịch bản trên cùng kho, in thống kê từng kịch bản | Chưa có bảng gộp 1 dòng/kịch bản, chưa lưu ra tệp; xong menu 9 thì phiên dừng ở kịch bản cuối (2 phòng, tắt ưu tiên), không trả lại cấu hình người dùng đã chọn |
| Dữ liệu | `patients_sample.txt` có 30 người: 29 người đến trong phút 0–13, 1 người đến phút 479 | Không có tải trải đều cả ngày nên không đủ để so sánh. Cần một bộ "trọn ngày" cố định |

**Việc tồn của giai đoạn 3** (làm ở mốc 0, song song):

- Chạy `mingw32-make test-all` và `test-console` trên Windows với `develop` mới.
- Kiểm tra chéo ca menu: A chạy ca của C, B chạy ca của A, C chạy ca của B.
- ASan: máy Leader không chạy được, cần một máy có ASan hoạt động (Linux/WSL hoặc macOS có Xcode mới).
- Chụp lại `a_tiep_nhan.png`: menu 4 giờ in thêm mã phòng.

### Những gì Leader đã thử trước khi lập kế hoạch

Bộ sinh hiện có (menu 2, seed 2026, giờ đóng 480, thời lượng 5–15) được chạy qua menu 9 với 3 cỡ dữ liệu:

| Số người | 1 phòng, bật ưu tiên: mức 2 chờ TB / max | 1 phòng, tắt ưu tiên: mức 0 chờ TB | 2 phòng: bật và tắt ưu tiên |
|---|---|---|---|
| 120 | 14.5 / 105 | 3.9 | Trùng khớp từng con số |
| 150 | 72.6 / 273 | 29.7 | Trùng khớp từng con số |
| 180 | 168.2 / 408 | 74.5 | Khác nhau (mức 1: 4.30 so với 5.03) |

Rút ra ba điều:

- **180 người** là cỡ hợp lý cho bộ trọn ngày. Ví dụ khoa Ngoại có 70 người với tổng khoảng 700 phút khám: quá tải khi có 1 phòng, vừa sức khi có 2 phòng. Nhờ vậy cả hai yếu tố (số phòng và ưu tiên) đều thấy rõ tác động.
- **Ở 120 và 150 người, 2 phòng bật/tắt ưu tiên cho số liệu giống hệt nhau.** Có thể đúng (hàng chờ mỗi phòng hiếm khi cùng lúc có cả người ưu tiên và người thường), nhưng phải chứng minh bằng số liệu trước khi đưa vào báo cáo.
- **Tuổi được sinh đều trong 0–120**, nên khoảng 48% là mức 1 (dưới 6 hoặc từ 60 tuổi), khác thực tế. Giai đoạn 4 **không đổi `DataGenerator`** để seed cũ vẫn ra đúng bộ cũ. Báo cáo cần ghi rõ giả định này; nếu muốn phân bố tuổi thực tế hơn thì bàn ở giai đoạn 5.

## 2. Phương án giữ nguyên

- Mọi quy tắc đã chốt ở [tổng quan giai đoạn 2](04_Tong_quan_giai_doan_2.md#1-phương-án-tiếp-tục-dùng) và giai đoạn 3: 3 khoa, 3 mức ưu tiên, heap/FIFO mỗi phòng, chọn phòng ít người chờ nhất (hòa thì chọn mã nhỏ), thứ tự trong một phút, nhận bệnh 0–479, không ngắt ca đang khám.
- **So sánh công bằng:** cả 4 kịch bản dùng đúng một kho bệnh nhân (cùng ticket, phút đến, thời lượng). Giữa hai kịch bản chỉ `initialize` lại, không nạp hay sinh lại dữ liệu.
- **Thời gian chờ là phút trong mô hình**, không phải thời gian máy chạy. Đo thời gian chạy để sang giai đoạn 5.
- Không đổi header nếu không cần. Nếu cần, báo Leader và sửa đồng bộ bên gọi trong cùng một PR. ConsoleUI không tự tính lại logic xếp hàng hay chọn phòng.

## 3. Những việc cần hoàn thành

1. **Bộ dữ liệu trọn ngày `data/patients_day.txt`:** 180 người, sinh bằng menu 2 với seed 2026, giờ đóng 480, thời lượng 5–15. Commit tệp và ghi lại seed/cấu hình. Đây là bộ chính thức cho mọi số liệu so sánh của giai đoạn 4.
2. **Nhật ký chạy:** menu 6 có lựa chọn ghi nhật ký từng phút ra tệp, ví dụ `results/log_<kịch bản>.txt`. Mỗi phút gồm snapshot các phòng, cộng dòng sự kiện: ai vừa hoàn tất, ai vừa được gọi, ở phòng nào. Ghi ra tệp không được in thêm gì lên màn hình; khi đo hiệu năng ở giai đoạn 5 sẽ tắt nhật ký.
3. **Thống kê đủ cho so sánh:** thêm chờ trung bình chung toàn hệ thống và phút kết thúc ngày (phút cuối còn người khám). Các số cũ giữ nguyên định nghĩa.
4. **Menu 9 ra bảng so sánh:**
   - In một bảng gộp, mỗi kịch bản một dòng: chờ TB/max của từng mức, chờ TB chung, hàng chờ lớn nhất của hệ thống, phút kết thúc, số bắt đầu/hoàn tất.
   - Lưu bảng vào `results/comparison.txt`.
   - Xong so sánh thì khởi tạo lại phiên theo cấu hình người dùng đã chọn trước đó.
5. **Kiểm chứng kết quả trước khi nhận xét:**
   - Ví dụ 3 người qua menu 9 vẫn cho 0/9/3 và 0/4/8.
   - Một bộ nhỏ được tính tay.
   - Các bất biến trên `patients_day.txt` ở cả 4 cấu hình (mục 6).
   - Giải thích được hiện tượng 2 phòng bật/tắt ưu tiên trùng số liệu.
6. **Bảng so sánh và nhận xét** trên `patients_day.txt`. Nhận xét phải dựa trên số liệu trong bảng, không suy đoán: ưu tiên làm thay đổi chờ của từng mức thế nào, thêm phòng làm thay đổi gì, và khi đủ phòng thì ưu tiên còn tác dụng không. Viết thành đoạn dùng được cho báo cáo.

**Chưa thuộc giai đoạn 4:** bộ 10.000 bản ghi, đo thời gian chạy và bộ nhớ, biểu đồ hiệu năng (giai đoạn 5); slide và báo cáo hoàn chỉnh (giai đoạn 6). Một biểu đồ cột chờ TB theo mức × kịch bản thì nên có, nhưng không bắt buộc.

## 4. Làm theo thứ tự nào?

| Mốc | Công việc | Điều kiện để sang mốc sau |
|---|---|---|
| 0. Trả nợ G3 | Chạy trên Windows, kiểm tra chéo ca menu, ASan nếu có máy | Có kết quả ghi vào bàn giao G3; máy nào chưa chạy thì ghi rõ |
| 1. Dữ liệu và số liệu nền | C commit `patients_day.txt`; B thêm chờ TB chung và phút kết thúc; Leader viết test bất biến | `make test-all` đạt với test mới |
| 2. Nhật ký và bảng | C làm nhật ký ra tệp và bảng menu 9 dùng hàm của B; A đối chiếu nhật ký với tính tay | Menu 9 trên `patients_day.txt` in bảng và lưu `results/comparison.txt` |
| 3. Phân tích | B lập bảng và viết nhận xét; Leader giải thích hiện tượng 2 phòng | Mọi số trong nhận xét truy ngược được về `results/` |
| 4. Duyệt | Chạy lại toàn bộ test và ca menu, kiểm tra chéo | Đủ tiêu chí mục 6 |

## 5. Giai đoạn 4 gửi gì cho Leader?

- Mã nguồn sửa/bổ sung, test trong `tests/`, ca menu trong `tests/console/` (có `.expected`, chạy được bằng `make test-console`).
- Tệp bàn giao trong **`docs/Giai Doan 4/`**, cùng mẫu với giai đoạn 3: đã làm gì, cách chạy, bảng ca → mong đợi → thực tế → đạt, lỗi còn lại.
- **Kết quả chính thức** từ `patients_day.txt` được commit vào `results/`: `comparison.txt` và một nhật ký mẫu (`log_1phong_uutien.txt`). Không commit đầu ra của test tự động hay thư mục `build/`.
- Ảnh console của menu 9 và một đoạn nhật ký, đặt trong `docs/Giai Doan 4/img/`.

## 6. Khi nào được duyệt xong giai đoạn 4?

- Việc tồn G3 đã làm hoặc được ghi rõ là chưa làm, kèm lý do.
- `make`, `make test-all`, `make test-console` đạt trên macOS và Windows; máy nào chưa thử phải ghi rõ.
- `patients_day.txt` (180 người) ở cả 4 cấu hình:
  - số bắt đầu = số hoàn tất = 180;
  - không ai bắt đầu trước phút đến;
  - `finish = start + thời lượng`;
  - mỗi phòng không có hai ca chồng nhau, mỗi người khám đúng khoa;
  - khi tắt ưu tiên, mỗi phòng gọi khám đúng thứ tự (phút đến, ticket).
- Nhật ký: số người chờ lớn nhất đọc từ nhật ký của mỗi phòng khớp `RoomStats.maxWaiting`. Sự kiện hoàn tất/gọi khám khớp `startTime`/`finishTime` trong `saveRunResults`.
- Menu 9 in bảng, lưu `results/comparison.txt` và trả phiên về cấu hình cũ. Ví dụ 3 người vẫn cho 0/9/3 và 0/4/8.
- Có giải thích bằng số liệu cho trường hợp 2 phòng bật/tắt ưu tiên trùng kết quả.
- Có bảng so sánh và nhận xét; mọi con số trong nhận xét có trong `results/comparison.txt`.

[Phân công giai đoạn 4](09_Phan_cong_giai_doan_4.md) · [Tổng quan giai đoạn 3](06_Tong_quan_giai_doan_3.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
