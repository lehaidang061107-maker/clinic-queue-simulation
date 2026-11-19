# TỔNG QUAN GIAI ĐOẠN 3

**Mục tiêu:** có một chương trình console hoàn chỉnh ở mức chức năng cơ bản: tiếp nhận, cấp số, phân phòng, gọi khám; nạp dữ liệu khi khởi động, lưu khi thoát, mở lại không mất người. Kiểm tra trên bộ mẫu ≥30 bản ghi.

## 1. Hiện trạng đầu giai đoạn

| Phần | Tình trạng | Việc phải xong trước |
|---|---|---|
| Leader: Patient, SimulationConfig, Simulation, main | Đã cài; có `G2_Leader_Ban_giao.md`; `test-leader-isolated` đạt 8/8 | Chạy lại `make test-leader` khi code heap/thống kê của B được merge |
| A: LinkedQueue, ClinicRoom | Đã cài; có `tests/test_a.cpp`, `make test-a-queue` đạt | Chạy `make test-a` đầy đủ khi có heap thật |
| B: PriorityHeap, Statistics, Merge Sort | Đã cài; có `tests/test_b.cpp` và `G2_B_Ban_giao.md` | Không còn việc tồn đọng |
| C: PatientStore, FileIO, DataGenerator, ConsoleUI | Đã cài; `make test-c` đạt trên macOS, 0 leak | Chạy lại trên Windows/MinGW-w64 |
| Dữ liệu | `patients_sample.txt`, `patients_large.txt` rỗng | Bộ mẫu ≥30 bản ghi làm trong giai đoạn này; bộ lớn để giai đoạn 5 |

**Giai đoạn 2 gần xong.** Việc tồn đọng của A/C/Leader đã làm ngày 19/11 trên macOS. Mốc 0 còn lại: chạy đủ test với mô-đun thật và build trên Windows.

## 2. Phương án giữ nguyên

- Mọi quy tắc đã chốt ở [tổng quan giai đoạn 2](04_Tong_quan_giai_doan_2.md#1-phương-án-tiếp-tục-dùng): 3 khoa, 3 mức ưu tiên, heap/FIFO mỗi phòng, chọn phòng ít người chờ nhất, thứ tự trong một phút, giờ nhận 0–479, không ngắt ca.
- Không đổi header nếu không cần. Nếu cần, báo Leader và sửa đồng bộ cả bên gọi trong cùng một PR.
- UI chỉ gọi Simulation/FileIO/PatientStore; không viết lại logic xếp hàng hoặc chọn phòng trong ConsoleUI.

## 3. Những việc cần hoàn thành

1. **Ghép đủ mô-đun thật:** `make` tạo được `build/clinic_simulator` từ `src/*.cpp`, không còn TODO, không còn cảnh báo từ mã nhóm với `-Wall -Wextra -Wpedantic`.
2. **Luồng khởi động/thoát:** khi chạy, chương trình nạp `data/patients_working.txt` nếu có (không có thì báo và bắt đầu kho rỗng); chọn 0 lưu đầu vào rồi thoát. Lần chạy sau thấy đủ người đã nhập thêm.
3. **Tiếp nhận thủ công qua menu:** nhập mã, tên, tuổi, cấp cứu có/không, khoa; chương trình tự cấp ticket, phút đến, mức ưu tiên và phòng; in lại ticket + phòng được phân cho người dùng.
4. **Gọi khám và xem trạng thái:** chạy từng phút hoặc chạy hết; xem mỗi phòng đang khám ai, còn bao nhiêu người chờ trong heap/FIFO; xem danh sách chờ đã sắp bằng Merge Sort.
5. **Dữ liệu mẫu ≥30 bản ghi** trong `data/patients_sample.txt`, phủ đủ khoa, mức ưu tiên, tuổi biên và nhiều người đến cùng phút.
6. **Kiểm tra bằng kịch bản nhập lặp lại được:** vì `runConsole` nhận `istream`, mỗi ca menu được lưu thành tệp nhập và chạy bằng `./build/clinic_simulator < tệp_nhập > tệp_kết_quả`, so với kết quả mong đợi.

Chưa thuộc giai đoạn 3: bảng so sánh 4 kịch bản có nhận xét (giai đoạn 4), bộ 10.000 bản ghi và đo thời gian chạy (giai đoạn 5). Menu 9 đã có thì chỉ cần chạy được, chưa cần kết luận.

## 4. Làm theo thứ tự nào?

| Mốc | Công việc | Điều kiện để sang mốc sau |
|---|---|---|
| 0. Trả nợ G2 | Chạy `make test-a`, `test-leader` với mô-đun thật; build Windows | Bốn test chạy đạt; `make` tạo được chương trình |
| 1. Luồng cơ bản | C làm nạp khi khởi động và bộ mẫu; Leader rà admit/phân phòng qua menu; B nối báo cáo chờ vào màn xem | Nạp mẫu → tiếp nhận → chạy → xem → lưu → thoát chạy trọn |
| 2. Kịch bản kiểm tra | Mỗi người viết kịch bản nhập cho phần mình, chạy và lưu đầu ra | Các ca ở [phân công](07_Phan_cong_giai_doan_3.md) có kết quả thực tế |
| 3. Duyệt | Kiểm tra chéo, chạy trên Windows và macOS, sửa lỗi tích hợp | Đủ tiêu chí mục 6 |

## 5. Giai đoạn 3 gửi gì cho Leader?

- Mã nguồn sửa/bổ sung, test trong `tests/`, kịch bản nhập trong `tests/console/`.
- Một tệp bàn giao `.md` trong **`docs/Giai Doan 3/`**: đã làm gì, lệnh chạy, bảng đầu vào → mong đợi → thực tế → đạt/chưa, lỗi còn lại.
- Ảnh hoặc đoạn đầu ra console của các luồng chính để dùng lại cho báo cáo/slide (ảnh đặt trong `docs/Giai Doan 3/img/`).
- Không commit thư mục `build/`, tệp thực thi hoặc tệp kết quả sinh ra khi chạy test tự động.

## 6. Khi nào được duyệt xong giai đoạn 3?

- Tiêu chí [giai đoạn 2](04_Tong_quan_giai_doan_2.md#5-khi-nào-được-duyệt-xong-giai-đoạn-2) đã đạt hết, có đủ 4 tệp bàn giao G2.
- `make` và `make test-a test-b test-c test-leader` chạy đạt trên ít nhất một máy macOS và một máy Windows (MinGW-w64); máy nào chưa thử phải ghi rõ.
- Khởi động có/không có `patients_working.txt` đều chạy được; tệp có dòng sai vẫn nạp các dòng đúng và báo số dòng lỗi.
- Nạp `patients_sample.txt` (≥30 người) → chạy hết ở cấu hình mặc định: số bắt đầu = số hoàn tất = số bệnh nhân; không ai bắt đầu trước phút đến.
- Tiếp nhận thủ công: ticket tự tăng, mã trùng/tuổi sai/khoa sai/nhập chữ bị từ chối và cho nhập lại, không thoát chương trình; phút 480 bị từ chối.
- Người tiếp nhận được phân đúng khoa, đúng phòng ít người chờ nhất; cấp cứu được gọi trước người thường đang chờ nhưng không ngắt ca đang khám.
- Lưu → thoát → mở lại: số người, mã, ticket, phút đến, thời lượng giống trước khi thoát; kết quả chạy nằm riêng trong `results/`.
- Lỗi ghi tệp (đường dẫn không ghi được) báo rõ, không báo “đã lưu”.
- Ví dụ 3 người qua menu vẫn cho chờ 0/9/3 (bật ưu tiên) và 0/4/8 (tắt ưu tiên).

[Phân công giai đoạn 3](07_Phan_cong_giai_doan_3.md) · [Tổng quan giai đoạn 2](04_Tong_quan_giai_doan_2.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
