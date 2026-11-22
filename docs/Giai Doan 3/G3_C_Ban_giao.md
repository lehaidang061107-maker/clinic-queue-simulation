# Bàn giao giai đoạn 3 — C

## Đã thực hiện

- Khi khởi động, chương trình nạp `data/patients_working.txt` nếu tệp tồn tại; nếu chưa có thì thông báo kho rỗng. Kết quả nạp hiển thị số dòng nhận/từ chối.
- Tiếp nhận thủ công cho phép nhập lại sau khi thông tin bị từ chối. Khi thành công, menu in ticket, mức ưu tiên và phút đến.
- Bổ sung `data/patients_sample.txt` gồm 30 bệnh nhân và `tests/data/invalid_mix.txt` gồm hai dòng hợp lệ xen sáu dòng sai.
- `test_c` kiểm tra độ phủ bộ mẫu và kết quả đọc tệp lỗi. Kịch bản menu nằm trong `tests/console/c_*.in`; tệp `.expected` liệt kê các thông báo chính cần đối chiếu.
- Điều chỉnh target `test-c` trong Makefile để liên kết mô-đun mô phỏng/thống kê thật, tương thích với linker MinGW trên Windows.
- README có hướng dẫn chạy từ thư mục gốc và làm rõ cách lưu/nạp dữ liệu phiên.

## Phủ dữ liệu mẫu

| Yêu cầu | Có trong tệp |
|---|---:|
| Số bản ghi | 30 |
| Chuyên khoa 1, 2, 3 | Có |
| Mức ưu tiên 0, 1, 2 | Có |
| Tuổi 0, 5, 6, 59, 60, 120 | Có |
| Ít nhất 3 người đến cùng phút | Có (4 người ở phút 0) |
| Đến phút 479 | Có |
| Thời lượng 5–15 phút | Có |

## Kiểm tra

| Kiểm tra | Mong đợi | Thực tế |
|---|---|---|
| Kiểm tra cấu trúc dữ liệu mẫu/lỗi | 30 và 8 bản ghi; mẫu đủ 8 trường và thời lượng hợp lệ | Đạt |
| `git diff --check` | Không có lỗi định dạng diff | Đạt |
| `mingw32-make all test-c test-leader` | Build và test C/Leader đạt | Đạt trên Windows, MSYS2 GCC 16.2.0, GNU Make 4.4.1 |
| `mingw32-make test-a test-b` | Test A/B đạt | Đạt trên cùng môi trường |
| Kịch bản `c_first_run`, `c_invalid_mix`, `c_admit_retry` | Các đoạn đầu ra trong `.expected` xuất hiện | Đạt; chạy trong thư mục sandbox riêng |
| Lưu/nạp lại qua hai lần chạy | Hai bệnh nhân được lưu, lần sau nạp lại đủ mã/ticket | Đạt trong sandbox; nạp lại 2 bệnh nhân |

## Điểm cần phối hợp

`Simulation::admit` hiện mới lưu bệnh nhân; phòng được gán khi `step()` xử lý phút đến. Vì vậy menu báo rõ phòng chưa phân thay vì tự tính/chọn phòng trong UI (tránh nhân đôi thuật toán phân phòng của Leader). Để in mã phòng ngay sau tiếp nhận như yêu cầu giai đoạn 3, cần Leader cập nhật luồng phân phòng hoặc giao diện `Simulation`.

Chạy các kịch bản từ thư mục gốc. Xóa hoặc di chuyển `data/patients_working.txt` trước mỗi ca để bảo đảm trạng thái khởi đầu rỗng. Windows đã được kiểm tra; macOS và kiểm tra chéo của thành viên khác vẫn chờ thực hiện.
