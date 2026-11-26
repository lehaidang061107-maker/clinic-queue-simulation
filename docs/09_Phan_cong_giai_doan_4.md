# PHÂN CÔNG GIAI ĐOẠN 4

Mỗi người làm mốc 0 trước, rồi việc giai đoạn 4. Làm trên nhánh riêng, mở PR vào `develop` theo [quy trình Git](../MEMBER_GIT_WORKFLOW.md). Ca menu đặt trong `tests/console/<người>_<ca>.in` và `.expected`; khi thoát lưu vào `build/console/<ca>_working.txt`, không lưu vào `data/patients_working.txt`.

## Leader — Kiểm chứng mô phỏng

Mốc 0: gom kết quả Windows và kiểm tra chéo giai đoạn 3; cập nhật README.

Giai đoạn 4:

1. **Phút kết thúc ngày:** thêm hàm lấy phút kết thúc, báo B và C.
2. **Test bất biến trọn ngày** trong `tests/test_leader.cpp`, 4 cấu hình trên bộ 180 người:
   - số bắt đầu = số hoàn tất = 180;
   - `start ≥ arrival`, `finish = start + thời lượng`;
   - đúng khoa, không chồng ca trong một phòng;
   - tắt ưu tiên: mỗi phòng đúng thứ tự (phút đến, ticket);
   - bật ưu tiên: không ai được gọi khi cùng phòng còn người mức cao hơn đang chờ.
3. **Giải thích 2 phòng bật/tắt ưu tiên trùng số liệu** (120 và 150 người): đếm số lần gọi vượt người đến trước. Bằng 0 thì hai chế độ chắc chắn cho cùng kết quả; khác 0 mà vẫn trùng thì có lỗi, phải tìm ra. Ghi bảng số liệu vào bàn giao.
4. **Rà menu 9:** 4 kịch bản dùng cùng một kho, thống kê được reset giữa các kịch bản, xong thì phiên trở về cấu hình cũ.
5. Nháp mục **Mô phỏng** cho báo cáo: thứ tự một phút, chọn phòng, khi nào kết thúc ngày, độ phức tạp.

**Ca `leader_*`:**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Ba người qua menu 9 | Nạp `tests/data/three_patients.txt` → menu 9 | 1 phòng bật ưu tiên 0/9/3; tắt 0/4/8; 2 phòng 0/4/0 |
| Trả phiên sau so sánh | 2 phòng/khoa bật ưu tiên → menu 9 → chạy hết | Lần chạy dùng 2 phòng/khoa bật ưu tiên, không phải cấu hình của kịch bản cuối |

**Gửi:** `G4_Leader_Ban_giao.md`.

## A — Đối chiếu nhật ký

Mốc 0: chạy `mingw32-make test-a` và `test-console` trên Windows; kiểm tra chéo ca `c_*`; chụp lại `a_tiep_nhan.png`.

Giai đoạn 4:

1. **Đối chiếu với tính tay:** theo dõi phòng Ngoại trong 60 phút đầu của nhật ký trên `patients_day.txt` (1 phòng/khoa, bật ưu tiên). Lập bảng phút → ai hoàn tất → ai được gọi → số chờ heap/FIFO.
2. **Hàng chờ lớn nhất:** lấy số người chờ lớn nhất của từng phòng từ nhật ký, so với `maxWaiting` trong `results/comparison.txt`, ở cả 1 phòng và 2 phòng/khoa. Lệch thì báo B và Leader.
3. **Không ngắt ca:** trong nhật ký, người đã vào khám giữ phòng đến đúng giờ kết thúc; kiểm một đoạn có cấp cứu đến lúc phòng bận.
4. Sơ đồ một phút (hoàn tất → tiếp nhận/xếp phòng → gọi khám); ảnh menu 9 và một đoạn nhật ký trong `docs/Giai Doan 4/img/`.

**Ca `a_*`:** nhật ký có cấp cứu đến khi phòng bận (dùng `three_patients.txt`), có dòng gọi cấp cứu ngay sau ca hiện tại.

**Gửi:** `G4_A_Ban_giao.md`, ảnh và sơ đồ.

## B — Số liệu so sánh và nhận xét

Mốc 0: chạy ca `a_*` của A, ghi vào `G3_A_Ban_giao.md`.

Giai đoạn 4:

1. **Chờ TB chung toàn hệ thống** trong `Statistics`: tổng chờ của 3 mức chia tổng số đã bắt đầu; chưa ai bắt đầu thì báo "Chua co du lieu", không chia cho 0. Test: bộ 3 người, bộ rỗng, bộ thiếu một mức. Báo C tên hàm trước khi C làm bảng.
2. **Tính tay** bộ `tests/data/format_example_2.txt` (5 người) cả 4 kịch bản (chờ TB/max từng mức, chờ TB chung), đối chiếu menu 9.
3. **Chạy chính thức** menu 9 trên `patients_day.txt`, commit `results/comparison.txt`. Ghi máy, ngày, commit.
4. **Bảng và nhận xét:**
   - bảng 4 dòng: chờ TB/max mức 0/1/2, chờ TB chung, hàng chờ lớn nhất, phút kết thúc;
   - 3 ý, mỗi ý dẫn số cụ thể: tác động của ưu tiên lên từng mức; tác động của số phòng; khi có 2 phòng thì ưu tiên còn khác biệt bao nhiêu;
   - ghi giả định: tuổi sinh đều 0–120 nên mức 1 chiếm khoảng 48%.

**Ca `b_*`:** menu 9 trên `format_example_2.txt` ra đúng bảng tính tay; kho rỗng hiện "Chua co du lieu".

**Gửi:** `G4_B_Ban_giao.md`, `results/comparison.txt`.

## C — Dữ liệu trọn ngày, nhật ký và bảng menu 9

Mốc 0: chạy ca `b_*` của B, ghi vào `G3_B_Ban_giao.md`.

Giai đoạn 4:

1. **`data/patients_day.txt`:** sinh bằng menu 2 (180 người, giờ đóng 480, thời lượng 5–15, seed 2026). Ghi bảng phân bố theo khoa, mức, giờ đến và tổng thời lượng mỗi khoa. Thêm vào `test_c.cpp` kiểm tra sinh lại cùng seed ra đúng tệp.
2. **Nhật ký ra tệp** (menu 6 hỏi có ghi không và đường dẫn, mặc định `results/log.txt`). Mỗi phút ghi:
   - `Phut m`;
   - snapshot các phòng;
   - `Hoan tat: <mã> (phong p)` cho người có `finishTime == m`;
   - `Goi kham: <mã> (phong p, cho w)` cho người có `startTime == m`.

   Lấy sự kiện từ trạng thái kho/phòng sau mỗi `step()`, không sửa `Simulation`. Ghi lỗi thì báo rõ và vẫn chạy hết. Commit `results/log_1phong_uutien.txt`.
3. **Menu 9:** giữ phần in chi tiết, thêm bảng gộp 1 dòng/kịch bản (chờ TB/max mức 0/1/2, chờ TB chung, hàng chờ lớn nhất, phút kết thúc, số bắt đầu/hoàn tất); lưu `results/comparison.txt` (hàm mới trong `FileIO`); xong thì trả phiên về cấu hình cũ.
4. Cập nhật mục Cách chạy trong README.

**Ca `c_*`:**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Nhật ký | Nạp `three_patients.txt` → 1 phòng bật ưu tiên → menu 6, ghi nhật ký | Tệp có `Phut 5`, `Hoan tat: BN01 (phong 1)`, `Goi kham: BN03 (phong 1, cho 3)` |
| Nhật ký không ghi được | Đường dẫn trong thư mục không tồn tại | Báo lỗi, vẫn chạy hết và in thống kê |
| Bảng so sánh | Nạp `three_patients.txt` → menu 9 | Bảng 4 dòng, lưu tệp kết quả |

**Gửi:** `G4_C_Ban_giao.md`, dữ liệu, nhật ký mẫu.

## Kiểm tra chéo

A chạy ca của B, B chạy ca của C, C chạy ca của A. Ít nhất một người chạy trên Windows.

```bash
make
make test-all
make test-console
```

Windows dùng `mingw32-make`; `test-console` cần `sh` của Git Bash hoặc MSYS2.

## Mẫu bàn giao

1. **Đã làm gì:** tệp sửa, ứng với việc nào.
2. **Cách chạy:** máy, hệ điều hành, compiler, lệnh, dữ liệu, commit.
3. **Kết quả:** bảng ca → mong đợi → thực tế → đạt; số liệu so sánh ghi rõ lấy từ tệp nào trong `results/`.
4. **Còn lại:** lỗi chưa sửa, việc chờ người khác.

[Tổng quan giai đoạn 4](08_Tong_quan_giai_doan_4.md)
