# Clinic Queue Simulator

Đề tài 6 — Mô phỏng hàng đợi khám bệnh bằng C++17, giao diện console trên Windows và macOS.

**Hiện trạng:** giai đoạn 3 đã hoàn thành và được merge vào `develop`. Giai đoạn 4 đang triển khai: C đã bổ sung nhật ký từng phút, bảng so sánh menu 9 và dữ liệu ngày 180 bệnh nhân; kết quả chính thức nằm trong `results/`. Bàn giao phần C ghi rõ môi trường và các kiểm tra đã chạy.

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
- `data/patients_day.txt`: 180 bệnh nhân cho so sánh G4, sinh bằng menu 2 với seed 2026, giờ đóng 480 và thời lượng 5–15 phút.
- `data/patients_large.txt`: dành cho thực nghiệm giai đoạn sau; hiện chưa có dữ liệu.
- `data/patients_working.txt`: dự kiến lưu đầu vào khi lưu/thoát; mở lại là phiên mới.
- `results/comparison.txt`: bảng so sánh 4 kịch bản trên `patients_day.txt`.
- `results/log_1phong_uutien.txt`: nhật ký theo phút của kịch bản 1 phòng/khoa, bật ưu tiên.

Dữ liệu sinh dùng seed để lặp lại được. Các kịch bản so sánh phải giữ nguyên đầu vào; chỉ ghi số liệu hiệu năng sau khi chạy đo thực tế.

## Cách chạy

Chạy lệnh từ thư mục gốc dự án. Cần C++17 và `make` (trên Windows: MinGW-w64 và `mingw32-make`):

```bash
make
make test-c
```

Chương trình nằm ở `build/clinic_simulator` (Windows: `build/clinic_simulator.exe`). Lần chạy đầu tự nạp `data/patients_working.txt` nếu tệp tồn tại; nếu chưa có thì bắt đầu với kho rỗng. Dùng mục 1 để nạp tệp khác, mục 2 để sinh dữ liệu, mục 3 để chọn cấu hình, mục 4–9 để tiếp nhận/chạy/xem/lưu/so sánh. Mục 6 có thể ghi nhật ký từng phút; mặc định lưu vào `results/log.txt`. Mục 9 chạy bốn cấu hình trên cùng dữ liệu, in bảng gộp và lưu mặc định vào `results/comparison.txt`; sau đó phiên được khởi tạo lại theo cấu hình đã chọn trước khi so sánh. Mục 0 lưu đầu vào vào `data/patients_working.txt` rồi thoát.

Các kịch bản nhập menu nằm trong `tests/console/` (`<người>_<ca>.in` kèm `<người>_<ca>.expected`). Chúng giả định chạy từ thư mục gốc và kho phiên ban đầu rỗng. `make test-console` chạy tất cả: trước mỗi ca, script tạm cất `data/patients_working.txt` rồi trả lại khi xong, và kiểm tra từng dòng trong `.expected` có xuất hiện theo đúng thứ tự trong đầu ra. Đầu ra mỗi ca nằm ở `build/console/<ca>.out`. Muốn chạy một ca bằng tay thì xóa hoặc di chuyển `data/patients_working.txt` trước:

```bash
./build/clinic_simulator < tests/console/leader_three_priority.in
```

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

Cần trình biên dịch C++17 (g++ hoặc clang++) và `make`. Chạy từ thư mục gốc:

```bash
make                 # build/clinic_simulator
make test-all        # test-a, test-b, test-c, test-leader với mô-đun thật
make test-console    # các kịch bản menu trong tests/console/
./build/clinic_simulator
```

| Máy | Compiler | Kết quả |
| --- | --- | --- |
| macOS 26.5 arm64 (Leader, 23/11/2026) | Apple clang 16.0.0, GNU Make 3.81 | `make`, `test-all`, `test-console` đạt |
| Windows (theo bàn giao G3 của C) | MSYS2 GCC 16.2.0, GNU Make 4.4.1 | `mingw32-make` và test A/B/C/Leader đạt; Leader chưa tự chạy lại |
| Windows (G4 C, checkout hiện tại) | MSYS2 GCC 16.2.0, Git Bash | Bốn unit test biên dịch/chạy đạt; `test-console`: 22/22. `make` chưa có trong PATH của môi trường này |

Trên Windows chạy `mingw32-make` rồi `build\clinic_simulator.exe < tests\console\leader_three_priority.in`. `make test-console` cần `sh` (có sẵn trong Git Bash/MSYS2). `make test-asan` build với `-fsanitize=address` rồi chạy test-leader và bộ mẫu ở 4 cấu hình. Target này cần một toolchain có ASan chạy được.

## Kế hoạch và tài liệu thiết kế

- [Tổng quan giai đoạn 4](docs/08_Tong_quan_giai_doan_4.md)
- [Phân công giai đoạn 4](docs/09_Phan_cong_giai_doan_4.md)
- [Bàn giao giai đoạn 4 — C](docs/Giai%20Doan%204/G4_C_Ban_giao.md)
- [Tổng quan giai đoạn 1](docs/02_Tong_quan_giai_doan_1.md)
- [Phân công giai đoạn 1](docs/03_Phan_cong_giai_doan_1.md)
- [Thiết kế Leader — mô phỏng và tích hợp](docs/Giai%20Doan%201/G1_Leader_Thiet_ke.md)
- [Thiết kế A — FIFO và phòng khám](docs/Giai%20Doan%201/G1_A_Thiet_ke.md)
- [Thiết kế B — heap và thống kê](docs/Giai%20Doan%201/G1_B_Thiet_ke.md)
- [Thiết kế C — kho, tệp và menu](docs/Giai%20Doan%201/G1_C_Thiet_ke.md)

Giai đoạn 2 triển khai mã nguồn theo các thiết kế trên, kiểm tra từng phần rồi tích hợp chương trình.
