# Clinic Queue Simulator

Đề tài 6 — Mô phỏng hàng đợi khám bệnh bằng C++17, giao diện console trên Windows và macOS.

**Hiện trạng:** các mô-đun Leader/A/B/C, menu console, bộ kiểm thử và dữ liệu mẫu 30 bệnh nhân đã có trong mã nguồn. Build và các test A/B/C/Leader đã chạy đạt trên Windows với MinGW-w64; macOS chưa xác minh trong lần bàn giao này.

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
- `data/patients_sample.txt`: 30 bản ghi, phủ đủ khoa/mức ưu tiên, tuổi biên, nhiều người đến cùng phút và phút 479.
- `data/patients_large.txt`: dành cho thực nghiệm giai đoạn sau; hiện chưa có dữ liệu.
- `data/patients_working.txt`: dự kiến lưu đầu vào khi lưu/thoát; mở lại là phiên mới.
- `results/`: kết quả và số liệu thực nghiệm, lưu riêng với đầu vào.

Dữ liệu sinh dùng seed để lặp lại được. Các kịch bản so sánh phải giữ nguyên đầu vào; chỉ ghi số liệu hiệu năng sau khi chạy đo thực tế.

## Cách chạy

Chạy lệnh từ thư mục gốc dự án. Cần C++17 và `make` (trên Windows: MinGW-w64 và `mingw32-make`):

```bash
make
make test-c
```

Chương trình nằm ở `build/clinic_simulator` (Windows: `build/clinic_simulator.exe`). Lần chạy đầu tự nạp `data/patients_working.txt` nếu tệp tồn tại; nếu chưa có thì bắt đầu với kho rỗng. Dùng mục 1 để nạp tệp khác, mục 2 để sinh dữ liệu, mục 3 để chọn cấu hình, mục 4–9 để tiếp nhận/chạy/xem/lưu/so sánh. Mục 0 lưu đầu vào vào `data/patients_working.txt` rồi thoát. Kết quả mô phỏng được lưu riêng trong `results/`.

Các kịch bản nhập menu của phần C nằm trong `tests/console/`. Chúng giả định chạy từ thư mục gốc và kho phiên ban đầu rỗng; xóa hoặc di chuyển `data/patients_working.txt` trước mỗi ca để tránh phụ thuộc dữ liệu của lần chạy trước.

## Cấu trúc và phân công

| Thư mục / thành viên | Nội dung |
| --- | --- |
| `include/`, `src/` | Khai báo và cài đặt các mô-đun |
| Leader | Patient, SimulationConfig, Simulation, main và tích hợp |
| A | LinkedQueue, ClinicRoom; tổng hợp slide |
| B | PriorityHeap, Statistics, Merge Sort và thực nghiệm |
| C | PatientStore, FileIO, ConsoleUI, DataGenerator; tổng hợp báo cáo |

Mỗi người bàn giao code, kiểm thử và phần báo cáo của mình. Thay đổi giao diện header cần báo Leader.

## Biên dịch

```bash
make
./build/clinic_simulator
```

Trên Windows chạy `mingw32-make` rồi mở `build\clinic_simulator.exe`. Dùng `make test-c` (Windows: `mingw32-make test-c`) để kiểm thử kho, nạp/lưu tệp, bộ sinh và dữ liệu C.

## Kế hoạch và tài liệu thiết kế

- [Tổng quan giai đoạn 1](docs/02_Tong_quan_giai_doan_1.md)
- [Phân công giai đoạn 1](docs/03_Phan_cong_giai_doan_1.md)
- [Thiết kế Leader — mô phỏng và tích hợp](docs/Giai%20Doan%201/G1_Leader_Thiet_ke.md)
- [Thiết kế A — FIFO và phòng khám](docs/Giai%20Doan%201/G1_A_Thiet_ke.md)
- [Thiết kế B — heap và thống kê](docs/Giai%20Doan%201/G1_B_Thiet_ke.md)
- [Thiết kế C — kho, tệp và menu](docs/Giai%20Doan%201/G1_C_Thiet_ke.md)

Giai đoạn 2 triển khai mã nguồn theo các thiết kế trên, kiểm tra từng phần rồi tích hợp chương trình.
