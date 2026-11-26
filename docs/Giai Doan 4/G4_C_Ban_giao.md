# Bàn giao giai đoạn 4 — C

## 1. Đã làm

- Tạo `data/patients_day.txt` qua menu 2: 180 bệnh nhân, đóng nhận phút 480, thời lượng 5–15 phút, seed 2026. Không chỉnh tay.
- Thêm nhật ký vào menu 6. Mỗi phút ghi snapshot phòng và các sự kiện hoàn tất/gọi khám sau `step()`. Đường dẫn mặc định là `results/log.txt`; lỗi mở/ghi được báo riêng và mô phỏng vẫn chạy hết.
- Bổ sung `Statistics::averageWait` để tính chờ trung bình toàn hệ thống, trả trạng thái không có dữ liệu an toàn; menu 9 dùng hàm này.
- Menu 9 giữ phần thống kê chi tiết, in và lưu bảng 4 kịch bản qua `saveComparisonResults`, mặc định vào `results/comparison.txt`; sau khi so sánh, khởi tạo lại phiên theo cấu hình trước đó.
- Thêm kiểm tra tái sinh chính xác bộ dữ liệu ngày trong `test_c`, test hàm lưu bảng trong `test_c`, test chờ TB trong `test_b`, và các ca console nhật ký/thất bại ghi/bảng so sánh/khôi phục cấu hình.
- Cập nhật cách chạy và đường dẫn đầu ra trong README.

## 2. Phân bố bộ dữ liệu ngày

| Thuộc tính | Phân bố |
|---|---|
| Tổng bệnh nhân | 180 |
| Theo khoa 1 / 2 / 3 | 59 / 51 / 70 |
| Theo mức ưu tiên 0 / 1 / 2 | 20 / 87 / 73 |
| Đến 00:00–01:00 / 01:00–02:00 / 02:00–03:00 / 03:00–04:00 | 23 / 20 / 26 / 25 |
| Đến 04:00–05:00 / 05:00–06:00 / 06:00–07:00 / 07:00–08:00 | 17 / 20 / 28 / 21 |
| Tổng thời lượng khám khoa 1 / 2 / 3 | 552 / 517 / 697 phút |

`tests/test_c.cpp` sinh lại bằng cùng cấu hình và so sánh toàn bộ tệp với `data/patients_day.txt`.

## 3. Kết quả chính thức

`results/comparison.txt` được sinh qua menu 9 từ bộ dữ liệu trên:

| Kịch bản | TB/max mức 0 | TB/max mức 1 | TB/max mức 2 | TB chung | Chờ lớn nhất | Phút kết thúc | Bắt đầu/hoàn tất |
|---|---:|---:|---:|---:|---:|---:|---:|
| 1 phòng, bật ưu tiên | 5.5 / 13 | 18.4943 / 50 | 168.164 / 408 | 77.75 | 41 | 723 | 180 / 180 |
| 1 phòng, tắt ưu tiên | 74.45 / 220 | 80.0345 / 241 | 74.9726 / 231 | 77.3611 | 43 | 723 | 180 / 180 |
| 2 phòng, bật ưu tiên | 2.55 / 8 | 4.29885 / 19 | 4.0274 / 32 | 3.99444 | 7 | 492 | 180 / 180 |
| 2 phòng, tắt ưu tiên | 2.55 / 8 | 5.03448 / 22 | 3.34247 / 16 | 4.07222 | 7 | 492 | 180 / 180 |

`results/log_1phong_uutien.txt` có 724 snapshot (phút 0–723), 180 sự kiện gọi khám và 180 sự kiện hoàn tất.

## 4. Kiểm tra

Môi trường: Windows, MSYS2 GCC 16.2.0; chạy kịch bản console bằng `sh` của Git Bash.

| Kiểm tra | Kết quả |
|---|---|
| Build chương trình với `-std=c++17 -Wall -Wextra -Wpedantic` | Đạt, không cảnh báo |
| `test-a`, `test-b`, `test-c`, `test-leader` (biên dịch và chạy trực tiếp bằng GCC) | Đạt |
| `sh tests/console/run_console_tests.sh` | 22 đạt, 0 chưa đạt |
| Sinh lại `patients_day.txt` trong `test_c` | Tệp trùng khớp |
| Nhật ký bộ ngày | 724 snapshot; 180 bắt đầu, 180 hoàn tất |
| Bốn cấu hình trên bộ ngày | Mỗi cấu hình bắt đầu/hoàn tất đủ 180 người; bất biến được xác nhận bởi `test_leader` |
| Ca nhật ký không mở được tệp | Báo lỗi và vẫn in thống kê hoàn tất |
| Ca menu 9 khôi phục cấu hình | Đạt qua `leader_restore_after_compare` |

Máy hiện tại không có `make`/`mingw32-make` trong PATH nên các binary unit test được biên dịch bằng đúng các nhóm nguồn của Makefile; `test-console` chạy bằng script chính thức. Chưa kiểm tra chéo trên macOS.

## 5. Cách tái tạo đầu ra

Từ thư mục gốc:

1. Chọn menu 1, nạp `data/patients_day.txt`.
2. Với `results/log_1phong_uutien.txt`: chọn menu 3, giữ giờ đóng 480, một phòng mỗi khoa, bật ưu tiên; chọn menu 6, không in từng phút, có ghi nhật ký và nhập đường dẫn trên.
3. Với `results/comparison.txt`: nạp lại cùng tệp rồi chọn menu 9, giờ đóng 480 và nhập đường dẫn trên.

Không commit thư mục `build/` hoặc các tệp dữ liệu phiên `patients_working.txt`.
