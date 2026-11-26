# TỔNG QUAN GIAI ĐOẠN 2

**Mục tiêu:** cài các cấu trúc dữ liệu theo thiết kế giai đoạn 1 thành code C++17 chạy được, kiểm tra từng phần trên dữ liệu nhỏ.

## 1. Cần làm gì?

| # | Việc | Kết quả cần có |
|---|---|---|
| 1 | Cấu trúc dữ liệu | FIFO, heap, kho bệnh nhân, Merge Sort chạy đúng, kể cả rỗng, hòa khóa, tăng mảng |
| 2 | Phòng khám và mô phỏng | Vòng từng phút và thống kê khớp ví dụ tính tay |
| 3 | Tệp, menu, bộ sinh | Đọc/ghi tệp, tiếp nhận thủ công, sinh dữ liệu theo seed |
| 4 | Ghép chương trình | Nạp/nhập → chạy từng phút hoặc chạy hết → thống kê → lưu và mở lại |
| 5 | Chạy thử 4 cấu hình | 1/2 phòng × bật/tắt ưu tiên trên cùng bộ nhỏ |
| 6 | Kiểm tra | Mỗi phần có chương trình test trong `tests/` |

## 2. Quy tắc

- Cài đúng hàm trong `include/`; code trong `src/`. Đổi header phải báo Leader.
- Chọn phòng đúng khoa có ít người chờ nhất (không tính người đang khám), hòa chọn mã nhỏ.
- Bật ưu tiên: mức 0/1 vào heap, mức 2 vào FIFO; tắt: tất cả vào FIFO.
- Mỗi phút: hoàn tất ca → nhận và xếp người đến → gọi khám → ghi trạng thái.
- Nhận phút 0–479, khám 5–15 phút, hết giờ vẫn khám hết.
- Kho sở hữu bệnh nhân; hàng, heap, phòng, báo cáo chỉ giữ con trỏ. Chạy lại thì reset trạng thái, giữ dữ liệu đầu vào.

## 3. Thứ tự làm

| Mốc | Việc | Xong khi |
|---|---|---|
| 1 | Leader: Patient, SimulationConfig; A: LinkedQueue; B: PriorityHeap; C: PatientStore | Từng phần biên dịch và test riêng được |
| 2 | A: ClinicRoom; B: Statistics, Merge Sort; Leader: Simulation | Ví dụ nhỏ đúng thứ tự khám và thống kê |
| 3 | C: FileIO, DataGenerator, ConsoleUI; Leader: main, Makefile | Chạy trọn từ menu đến lưu tệp |
| 4 | Kiểm tra chéo, chạy trên Windows và macOS | Đủ tiêu chí mục 5 |

## 4. Nộp gì?

- Mã `src/*.cpp` phần mình, test trong `tests/`.
- Bàn giao `docs/Giai Doan 2/G2_<người>_Ban_giao.md`.

## 5. Tiêu chí duyệt

- Biên dịch C++17 không cảnh báo với `-Wall -Wextra -Wpedantic`.
- Ví dụ 3 người: bật ưu tiên chờ 0/9/3, tắt ưu tiên 0/4/8.
- Từ chối dữ liệu sai; xử lý thiếu tệp, dòng sai, lỗi ghi; nhận phút 479, từ chối phút 480.
- Chạy hết sau giờ đóng; số bắt đầu = số hoàn tất = số bệnh nhân; chạy lại không cộng dồn.
- Lưu rồi nạp lại không mất người; kết quả lưu riêng trong `results/`.
- Reset và hủy không xóa nhầm bệnh nhân, không để con trỏ treo.
- Chạy được trên Windows và macOS.

[Phân công giai đoạn 2](05_Phan_cong_giai_doan_2.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
