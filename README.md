# Clinic Queue Simulator

Đề tài 6 — Mô phỏng hàng đợi khám bệnh bằng C++17, giao diện console trên Windows và macOS.

**Hiện trạng:** đã có thiết kế giai đoạn 1 cho Leader/A/B/C, khai báo header, khung `.cpp` và ví dụ 5 bệnh nhân. Bước tiếp theo là cài đặt và kiểm tra ở giai đoạn 2; chưa có ứng dụng chạy được.

## Mục tiêu

Tiếp nhận bệnh nhân, điều phối phòng khám, mô phỏng theo phút và thống kê thời gian chờ. So sánh 4 kịch bản trên cùng dữ liệu: 1 hoặc 2 phòng/khoa, có hoặc không ưu tiên.

## Quy tắc chính

- Có 3 chuyên khoa: Nội, Nhi, Ngoại. Chọn phòng đúng khoa có ít người chờ nhất; hòa thì chọn mã phòng nhỏ nhất. Không chuyển phòng sau khi xếp hàng.
- Ưu tiên: `0` cấp cứu; `1` người dưới 6 hoặc từ 60 tuổi; `2` thông thường. Tuổi hợp lệ 0–120; chuyên khoa được nhập riêng.
- Mỗi phòng có FIFO bằng danh sách liên kết và min-heap bằng mảng động, đều tự cài đặt. Khi bật ưu tiên, mức 0/1 vào heap, mức 2 vào FIFO; tắt ưu tiên thì tất cả vào FIFO.
- Heap so sánh theo mức ưu tiên → phút đến → số thứ tự. Phòng rảnh lấy heap trước FIFO; không gián đoạn ca đang khám.
- Mỗi phút: hoàn tất ca → tiếp nhận người đến → gọi người tiếp theo. Mặc định nhận bệnh trong phút 0–479, sau đó khám hết người còn lại.
- Mã bệnh nhân và số thứ tự phải duy nhất. Thời lượng khám là số phút nguyên dương. Dữ liệu sai phải báo lỗi.
- Thống kê chờ trung bình/lớn nhất theo mức ưu tiên và số người chờ lớn nhất. Dùng Merge Sort tự cài để liệt kê thời gian chờ giảm dần, hòa theo số thứ tự tăng dần.
- `PatientStore` sở hữu bệnh nhân; hàng đợi, heap và phòng chỉ giữ con trỏ. Dọn các tham chiếu trước khi xóa kho; reset trạng thái khi chạy kịch bản mới.
- Không dùng container STL hoặc `sort`/`find` cho phần lõi; được dùng chuỗi và luồng nhập/xuất. FIFO thêm/lấy O(1); heap O(log n), riêng lần tăng mảng O(n); Merge Sort O(n log n).

## Dữ liệu

Tệp UTF-8, phân cách bằng `|`, có dòng tiêu đề:

```text
id|fullName|age|priority|specialtyId|ticketNumber|arrivalTime|serviceDuration
BN01|Nguyen Van An|30|2|1|1|0|5
```

- `data/format_example.txt`: ví dụ 5 bệnh nhân.
- `data/patients_sample.txt`, `data/patients_large.txt`: dự kiến ≥30 và ≥10.000 bản ghi, hiện chưa có dữ liệu.
- `data/patients_working.txt`: dự kiến lưu đầu vào khi lưu/thoát; mở lại là phiên mới.
- `results/`: kết quả và số liệu thực nghiệm, lưu riêng với đầu vào.

Dữ liệu sinh dùng seed để lặp lại được. Các kịch bản so sánh phải giữ nguyên đầu vào; chỉ ghi số liệu hiệu năng sau khi chạy đo thực tế.

## Cấu trúc và phân công

| Thư mục / thành viên | Nội dung |
| --- | --- |
| `include/`, `src/` | Khai báo và cài đặt các mô-đun |
| Leader | Patient, SimulationConfig, Simulation, main và tích hợp |
| A | LinkedQueue, ClinicRoom; tổng hợp slide |
| B | PriorityHeap, Statistics, Merge Sort và thực nghiệm |
| C | PatientStore, FileIO, ConsoleUI, DataGenerator; tổng hợp báo cáo |

Mỗi người bàn giao code, kiểm thử và phần báo cáo của mình. Thay đổi giao diện header cần báo Leader.

## Biên dịch dự kiến

Chạy từ thư mục gốc sau khi hoàn thiện mã nguồn; lệnh hiện chưa tạo được ứng dụng chạy được:

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/*.cpp -o build/clinic_simulator
./build/clinic_simulator
```

Trên Windows dùng MinGW-w64, tạo thư mục `build` và đổi tên đầu ra thành `build/clinic_simulator.exe`.

## Kế hoạch và tài liệu thiết kế

- [Tổng quan giai đoạn 1](docs/02_Tong_quan_giai_doan_1.md)
- [Phân công giai đoạn 1](docs/03_Phan_cong_giai_doan_1.md)
- [Thiết kế Leader — mô phỏng và tích hợp](docs/Giai%20Doan%201/G1_Leader_Thiet_ke.md)
- [Thiết kế A — FIFO và phòng khám](docs/Giai%20Doan%201/G1_A_Thiet_ke.md)
- [Thiết kế B — heap và thống kê](docs/Giai%20Doan%201/G1_B_Thiet_ke.md)
- [Thiết kế C — kho, tệp và menu](docs/Giai%20Doan%201/G1_C_Thiet_ke.md)

Giai đoạn 2 triển khai mã nguồn theo các thiết kế trên, kiểm tra từng phần rồi tích hợp chương trình.
