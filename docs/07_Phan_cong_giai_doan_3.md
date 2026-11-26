# PHÂN CÔNG GIAI ĐOẠN 3

Mỗi người làm mốc 0 trước, rồi việc giai đoạn 3. Làm trên nhánh riêng, mở PR vào `develop` theo [quy trình Git](../MEMBER_GIT_WORKFLOW.md). Mỗi ca menu gồm `tests/console/<người>_<ca>.in` và `.expected`, tự chứa: nạp một tệp cố định, kết thúc bằng lựa chọn thoát, không dựa vào `patients_working.txt` còn sót.

## Leader — Tích hợp và phân phòng

Mốc 0:

1. Chạy `make test-leader` với mô-đun thật của A/B/C, sửa lỗi ghép; thêm `test-b`, `test-all` vào Makefile.
2. Duyệt PR mốc 0; chỉ sang mốc 1 khi `make` tạo được chương trình.

Giai đoạn 3:

1. Rà luồng menu 4 → `admit`: tự cấp ticket, phút đến = phút hiện tại, xếp phòng ngay; in ticket, mức ưu tiên, mã phòng.
2. Kiểm tra 2 phòng/khoa: hòa số chờ chọn phòng mã nhỏ; người đang khám không tính vào số chờ.
3. Chạy ví dụ 3 người ở hai chế độ: 0/9/3 và 0/4/8.
4. Chạy `patients_sample.txt` ở 4 cấu hình: số bắt đầu = hoàn tất = số người.
5. Build và chạy trên macOS và Windows; chạy AddressSanitizer (hoặc công cụ thay thế nếu máy không hỗ trợ), ghi kết quả.
6. Cập nhật mục Hiện trạng và Biên dịch trong README.

**Ca `leader_*`:**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Ba người, bật ưu tiên | Nạp 3 người → 1 phòng/khoa, bật ưu tiên → chạy hết → xem | Chờ 0/9/3 |
| Ba người, tắt ưu tiên | Như trên, tắt ưu tiên | 0/4/8 |
| Hòa phòng | 2 phòng Nội, tiếp nhận 2 người Nội cùng phút khi cả hai phòng trống | Người 1 vào phòng mã nhỏ, người 2 vào phòng kia |
| Cấp cứu khi bận | Phòng đang khám, có 1 người thường chờ, tiếp nhận 1 cấp cứu | Cấp cứu được gọi ngay sau ca hiện tại, trước người thường |
| Đóng nhận | Chạy tới phút 479 tiếp nhận được; sang 480 bị từ chối | Báo đã đóng nhận, vẫn khám hết người chờ |

**Gửi:** `G3_Leader_Ban_giao.md`.

## A — Trạng thái phòng

Mốc 0: chạy `make test-a` đầy đủ với heap thật và trên Windows; cập nhật `G2_A_Ban_giao.md`.

Giai đoạn 3:

1. Kiểm tra snapshot (menu 5, 7): mỗi phòng hiện người đang khám, giờ xong, số chờ heap và FIFO; phòng trống hiện rõ.
2. Theo dõi 20 phút đầu của bộ mẫu: lập bảng phút → ai hoàn tất → ai được gọi, đối chiếu tính tay.
3. Tắt ưu tiên: heap luôn rỗng, mọi người qua FIFO theo thứ tự đến.
4. Chụp ảnh console các luồng chính (menu, tiếp nhận, snapshot, thống kê, lưu/thoát) vào `docs/Giai Doan 3/img/`.

**Ca `a_*`:** phòng trống; phòng bận có cả heap và FIFO; tắt ưu tiên; 20 phút đầu bộ mẫu.

**Gửi:** `G3_A_Ban_giao.md`.

## B — Thống kê và báo cáo chờ

Giai đoạn 3 (B không có việc tồn ở mốc 0):

1. Menu 7 sau khi chạy hết bộ mẫu: mỗi mức có số người, chờ TB, chờ lớn nhất; mức không có ai hiện "Chua co du lieu".
2. Danh sách chờ giảm dần, hòa theo ticket tăng dần; người chưa bắt đầu không xuất hiện; chạy giữa chừng thì người đang khám có trong danh sách.
3. Chạy lại cùng kịch bản hai lần: thống kê lần hai giống lần một.
4. `results/run_results.txt` khớp màn thống kê.

**Ca `b_*`:** bộ 3 người hai chế độ; chạy lại hai lần; xem thống kê giữa chừng; bộ không có mức 0 hiện "Chua co du lieu".

**Gửi:** `G3_B_Ban_giao.md`.

## C — Khởi động, thoát và dữ liệu mẫu

Mốc 0: chạy `mingw32-make test-c` trên Windows; bổ sung vào `G2_C_Ban_giao.md`.

Giai đoạn 3:

1. **Nạp khi khởi động:** có `data/patients_working.txt` thì nạp và in số người nhận/từ chối; không có thì báo chưa có dữ liệu và chạy với kho rỗng.
2. **Lưu khi thoát:** chọn 0 lưu vào `patients_working.txt`; lưu thất bại thì hỏi lại; EOF không làm treo.
3. **`data/patients_sample.txt` ≥30 bản ghi:** đủ 3 khoa; đủ mức 0/1/2; tuổi 0, 5, 6, 59, 60, 120; ≥3 người cùng phút; 1 người đến phút 479; thời lượng 5–15. Ghi bảng phủ ca vào bàn giao.
4. Tệp lỗi `tests/data/invalid_mix.txt` (dòng đúng xen dòng sai).
5. Thông báo menu nói rõ sai gì và cho nhập lại; sau tiếp nhận in ticket, mức, phòng.
6. Viết nháp mục Cách chạy cho README.

**Ca `c_*`:**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Lần đầu chạy | Không có `patients_working.txt` → thoát | Báo chưa có dữ liệu, thoát bình thường |
| Lưu/mở lại | Nạp mẫu → tiếp nhận 2 người → thoát → chạy lại | Đủ số người, 2 người mới có ticket nối tiếp |
| Nhập sai | Tuổi "abc", tuổi −1, khoa 4, mã trùng | Từng lỗi bị từ chối, cho nhập lại |
| Tệp lỗi | Nạp `invalid_mix.txt` | Nhận dòng đúng, báo số dòng và lý do dòng sai |
| Không ghi được | Lưu vào thư mục không tồn tại | Báo lỗi, không báo "đã lưu" |
| EOF | Tệp nhập dừng giữa chừng | Kết thúc, không lặp vô hạn |

**Gửi:** `G3_C_Ban_giao.md`, dữ liệu mẫu.

## Kiểm tra chéo

A chạy ca của C, B chạy ca của A, C chạy ca của B. Ít nhất một người chạy trên Windows.

```bash
make
make test-all
make test-console
```

Windows dùng `mingw32-make`; `test-console` cần `sh` của Git Bash hoặc MSYS2.

## Mẫu bàn giao

1. **Đã làm gì:** tệp sửa, ứng với việc nào.
2. **Cách chạy:** máy, hệ điều hành, compiler, lệnh.
3. **Kết quả:** bảng ca → mong đợi → thực tế → đạt.
4. **Còn lại:** lỗi chưa sửa, việc chờ người khác.

[Tổng quan giai đoạn 3](06_Tong_quan_giai_doan_3.md)
