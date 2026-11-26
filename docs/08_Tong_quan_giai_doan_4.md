# TỔNG QUAN GIAI ĐOẠN 4

**Mục tiêu:** chạy mô phỏng trọn một ngày (nhận bệnh phút 0–479, sau đó khám hết người còn lại), in và ghi trạng thái từng phút, rồi so sánh 4 kịch bản trên cùng một bộ dữ liệu: 1 hoặc 2 phòng/khoa, bật hoặc tắt ưu tiên.

## 1. Cần làm gì?

| # | Việc | Kết quả cần có |
|---|---|---|
| 1 | Bộ dữ liệu trọn ngày | `data/patients_day.txt`: 180 người, menu 2, seed 2026, giờ đóng 480, thời lượng 5–15; không chỉnh tay |
| 2 | Nhật ký từng phút | Menu 6 ghi ra tệp: snapshot các phòng, người vừa hoàn tất, người vừa được gọi (kèm phòng và thời gian chờ) |
| 3 | Thống kê đủ để so sánh | Thêm chờ trung bình chung toàn hệ thống và phút kết thúc ngày; số cũ giữ nguyên định nghĩa |
| 4 | Bảng so sánh ở menu 9 | Mỗi kịch bản một dòng; lưu `results/comparison.txt`; xong thì trả phiên về cấu hình cũ |
| 5 | Kiểm chứng | Bất biến trên bộ 180 người ở cả 4 cấu hình; đối chiếu nhật ký với tính tay |
| 6 | Nhận xét | Dựa trên số liệu trong bảng: ưu tiên đổi chờ từng mức thế nào, thêm phòng đổi gì, đủ phòng thì ưu tiên còn tác dụng không |

## 2. Quy tắc

- Giữ mọi quy tắc đã chốt ở giai đoạn 2–3.
- **So sánh công bằng:** cả 4 kịch bản dùng đúng một kho bệnh nhân. Giữa các kịch bản chỉ `initialize` lại, không nạp hay sinh lại dữ liệu.
- Thời gian chờ là phút trong mô hình, không phải thời gian máy chạy.
- Không đổi `DataGenerator` để seed 2026 vẫn ra đúng bộ đã commit.
- Không đổi header nếu không cần; nếu đổi thì báo Leader và sửa cả bên gọi trong cùng PR.

## 3. Thứ tự làm

| Mốc | Việc | Xong khi |
|---|---|---|
| 0 | Chạy Windows và kiểm tra chéo ca menu của giai đoạn 3 | Có kết quả ghi vào bàn giao G3 |
| 1 | C: `patients_day.txt`; B: chờ TB chung; Leader: phút kết thúc và test bất biến | `make test-all` đạt |
| 2 | C: nhật ký và bảng menu 9; A: đối chiếu nhật ký với tính tay | Menu 9 in bảng và lưu `results/comparison.txt` |
| 3 | B: bảng và nhận xét; Leader: giải thích 2 phòng bật/tắt ưu tiên trùng số liệu | Mọi số trong nhận xét truy ngược được về `results/` |
| 4 | Chạy lại toàn bộ test và ca menu, kiểm tra chéo | Đủ tiêu chí mục 5 |

## 4. Nộp gì?

- Mã, test trong `tests/`, ca menu trong `tests/console/`.
- Bàn giao `docs/Giai Doan 4/G4_<người>_Ban_giao.md`; ảnh trong `docs/Giai Doan 4/img/`.
- Kết quả chính thức từ `patients_day.txt`, commit vào `results/`: `comparison.txt`, `log_1phong_uutien.txt`. Không commit `build/` hay đầu ra test tự động.

## 5. Tiêu chí duyệt

- `make`, `make test-all`, `make test-console` đạt trên macOS và Windows (máy nào chưa thử phải ghi rõ).
- Trên `patients_day.txt` ở cả 4 cấu hình: số bắt đầu = số hoàn tất = 180; không ai bắt đầu trước phút đến; `finish = start + thời lượng`; không có hai ca chồng nhau trong một phòng; mỗi người khám đúng khoa; tắt ưu tiên thì mỗi phòng gọi đúng thứ tự (phút đến, ticket).
- Nhật ký: số người chờ lớn nhất đọc từ nhật ký khớp `maxWaiting`; sự kiện hoàn tất/gọi khám khớp `startTime`/`finishTime`.
- Menu 9 in bảng, lưu `results/comparison.txt`, trả phiên về cấu hình cũ; ví dụ 3 người vẫn cho 0/9/3 và 0/4/8.
- Có giải thích bằng số liệu cho trường hợp 2 phòng bật/tắt ưu tiên trùng kết quả.
- Mọi con số trong nhận xét có trong `results/comparison.txt`.

[Phân công giai đoạn 4](09_Phan_cong_giai_doan_4.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
