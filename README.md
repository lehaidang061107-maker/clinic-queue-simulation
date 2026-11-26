# Clinic Queue Simulator

Đề tài 6 — Mô phỏng hàng đợi khám bệnh bằng C++17, giao diện console trên Windows và macOS.

**Hiện trạng (26/11/2026):** giai đoạn 1–4 đã merge vào `develop`. Chương trình chạy trọn ngày, ghi nhật ký từng phút và so sánh 4 kịch bản; kết quả chính thức nằm trong `results/`. Giai đoạn 5 (kiểm thử, bộ 10.000 bản ghi, đo hiệu năng và bộ nhớ) bắt đầu; phần giai đoạn 4 của B từng bị một commit tích hợp ghi đè đã được khôi phục. Xem [tổng quan giai đoạn 5](docs/11_Tong_quan_giai_doan_5.md).

## Mục tiêu

Tiếp nhận bệnh nhân, điều phối phòng khám, mô phỏng theo phút và thống kê thời gian chờ. So sánh 4 kịch bản trên cùng dữ liệu: 1 hoặc 2 phòng/khoa, có hoặc không ưu tiên.

## Quy tắc chính

- Có 3 chuyên khoa: Nội, Nhi, Ngoại. Chọn phòng đúng khoa có ít người chờ nhất; hòa thì chọn mã phòng nhỏ nhất. Không chuyển phòng sau khi xếp hàng.
- Ưu tiên: `0` cấp cứu; `1` người dưới 6 hoặc từ 60 tuổi; `2` thông thường. Tuổi hợp lệ 0–120; chuyên khoa được nhập riêng.
- Mỗi phòng có FIFO bằng danh sách liên kết và min-heap bằng mảng động, đều tự cài đặt. Khi bật ưu tiên, mức 0/1 vào heap, mức 2 vào FIFO; tắt ưu tiên thì tất cả vào FIFO.
- Heap so sánh theo mức ưu tiên → phút đến → số thứ tự. Phòng rảnh lấy heap trước FIFO; không gián đoạn ca đang khám.
- Mỗi phút: hoàn tất ca → tiếp nhận người đến → gọi người tiếp theo. Mặc định nhận bệnh trong phút 0–479, sau đó khám hết người còn lại.
- Mã bệnh nhân và số thứ tự phải duy nhất. Thời lượng khám là số phút nguyên dương. Dữ liệu sai phải báo lỗi.
- Thống kê chờ trung bình/lớn nhất theo mức ưu tiên, chờ trung bình chung, số người chờ lớn nhất và phút kết thúc ngày. Dùng Merge Sort tự cài để liệt kê thời gian chờ giảm dần, hòa theo số thứ tự tăng dần.
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
- `data/patients_large.txt`: 10.000 bản ghi cho thực nghiệm G5 (seed 2026, giờ đóng 480, thời lượng 5–15); hiện chưa có dữ liệu, C sinh ở giai đoạn 5.
- `data/patients_working.txt`: lưu đầu vào khi thoát; lần chạy sau tự nạp lại.
- `results/comparison.txt`: bảng so sánh 4 kịch bản trên `patients_day.txt`.
- `results/log_1phong_uutien.txt`: nhật ký theo phút của kịch bản 1 phòng/khoa, bật ưu tiên.

Dữ liệu sinh dùng seed để lặp lại được. Các kịch bản so sánh phải giữ nguyên đầu vào; chỉ ghi số liệu hiệu năng sau khi chạy đo thực tế. Bộ sinh chọn tuổi đều trong 0–120 nên khoảng 48% bệnh nhân là mức 1; đây là giả định của mô hình, không phải phân bố thực tế.

## Hướng dẫn chạy nhanh

Yêu cầu: trình biên dịch C++17 (g++ hoặc clang++) và `make` (Windows: MinGW-w64 và `mingw32-make`).

1. Mở terminal và vào thư mục gốc dự án:
   ```bash
   cd "/đường/dẫn/tới/Lab"
   ```
2. Biên dịch:
   ```bash
   make                # Windows: mingw32-make
   ```
3. (Tùy chọn) Chạy test để kiểm tra:
   ```bash
   make test-all
   make test-console
   ```
4. Chạy chương trình:
   ```bash
   ./build/clinic_simulator          # Windows: build\clinic_simulator.exe
   ```
5. Dùng menu theo thứ tự gợi ý:
   - Mục `1`: nạp dữ liệu, nhập `data/patients_sample.txt` (30 bệnh nhân mẫu) hoặc `data/patients_day.txt` (180 người trọn ngày). Hoặc mục `2` để sinh dữ liệu ngẫu nhiên theo seed.
   - Mục `3`: chọn cấu hình (1 hoặc 2 phòng/khoa, bật hoặc tắt ưu tiên) rồi khởi tạo.
   - Mục `5`: chạy một phút. Mục `6`: chạy đến hết, có thể ghi nhật ký từng phút ra tệp.
   - Mục `7`: xem phòng và thống kê thời gian chờ.
   - Mục `9`: so sánh 4 kịch bản trên cùng dữ liệu, in bảng và lưu vào `results/comparison.txt`.
   - Mục `8`: lưu dữ liệu đầu vào hoặc kết quả (kết quả nằm trong `results/`).
   - Mục `0`: lưu đầu vào vào `data/patients_working.txt` rồi thoát.

Chạy nhanh không cần gõ menu, dùng một kịch bản có sẵn:

```bash
./build/clinic_simulator < tests/console/leader_three_priority.in
```

(Xóa hoặc di chuyển `data/patients_working.txt` trước nếu tệp này đã tồn tại.)

## Cách chạy

Chạy lệnh từ thư mục gốc dự án. Cần C++17 và `make` (trên Windows: MinGW-w64 và `mingw32-make`):

```bash
make
make test-c
```

Chương trình nằm ở `build/clinic_simulator` (Windows: `build/clinic_simulator.exe`). Lần chạy đầu tự nạp `data/patients_working.txt` nếu tệp tồn tại; nếu chưa có thì bắt đầu với kho rỗng. Dùng mục 1 để nạp tệp khác, mục 2 để sinh dữ liệu, mục 3 để chọn cấu hình, mục 4–9 để tiếp nhận/chạy/xem/lưu/so sánh. Mục 6 có thể ghi nhật ký từng phút; mặc định lưu vào `results/log.txt`. Mục 9 chạy bốn cấu hình trên cùng dữ liệu, in bảng gộp và lưu mặc định vào `results/comparison.txt`; sau đó phiên được khởi tạo lại theo cấu hình đã chọn trước khi so sánh. Mục 0 lưu đầu vào vào `data/patients_working.txt` rồi thoát.

**Tái tạo kết quả chính thức** từ `data/patients_day.txt`:

- `results/comparison.txt`: mục 1 nạp tệp → mục 9, giờ đóng 480, nhập đường dẫn trên.
- `results/log_1phong_uutien.txt`: mục 1 nạp tệp → mục 3 (giờ đóng 480, 1 phòng/khoa, bật ưu tiên) → mục 6, không in từng phút, có ghi nhật ký vào đường dẫn trên.

Các kịch bản nhập menu nằm trong `tests/console/` (`<người>_<ca>.in` kèm `<người>_<ca>.expected`). Chúng giả định chạy từ thư mục gốc và kho phiên ban đầu rỗng. `make test-console` chạy tất cả: trước mỗi ca, script tạm cất `data/patients_working.txt` rồi trả lại khi xong, và kiểm tra từng dòng trong `.expected` có xuất hiện theo đúng thứ tự trong đầu ra. Đầu ra mỗi ca nằm ở `build/console/<ca>.out`. Muốn chạy một ca bằng tay thì xóa hoặc di chuyển `data/patients_working.txt` trước:

```bash
./build/clinic_simulator < tests/console/leader_three_priority.in
```

## Cấu trúc và phân công

| Thư mục / thành viên | Nội dung |
| --- | --- |
| `include/`, `src/` | Khai báo và cài đặt các mô-đun |
| `tests/` | Unit test từng thành viên, `tests/console/` (kịch bản menu), `tests/data/` (dữ liệu nhỏ cho test) |
| `results/` | Kết quả chính thức đã commit (bảng so sánh, nhật ký mẫu; G5 thêm số đo hiệu năng) |
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
make test-asan       # AddressSanitizer: cần Linux/WSL (xem bảng dưới)
./build/clinic_simulator
```

| Máy | Compiler | Kết quả |
| --- | --- | --- |
| macOS 26.5 arm64 (Leader, 26/11/2026, sau G4 và khôi phục phần B) | Apple clang 16.0.0, GNU Make 3.81 | `make` không cảnh báo; `test-all` đạt; `test-console` 25/25; `leaks` 0 rò rỉ; UBSan không lỗi. `test-asan` **không chạy được**: ASan dừng ngay lúc khởi động (lỗi toolchain, chưa chạy tới code nhóm) |
| Windows (G4 C, trước khi gộp A/B) | MSYS2 GCC 16.2.0, Git Bash | Bốn unit test biên dịch tay đạt; `test-console` 22/22. `make` chưa có trong PATH |
| Windows (G4 A, MSYS2 UCRT64) | GCC, `mingw32-make` | `test-a` và `test-console` đạt |
| Windows, `develop` sau G4 | — | **Chưa chạy** (C làm ở mốc 0 giai đoạn 5) |
| Linux/WSL, `make test-asan` | — | **Chưa chạy** (C làm ở mốc 0 giai đoạn 5) |

Trên Windows chạy `mingw32-make` rồi `build\clinic_simulator.exe < tests\console\leader_three_priority.in`. `make test-console` cần `sh` (có sẵn trong Git Bash/MSYS2). `make test-asan` build với `-fsanitize=address` rồi chạy test-leader và bộ mẫu ở 4 cấu hình; MinGW không hỗ trợ ASan nên chạy trong WSL Ubuntu.

## Tổng hợp công việc giai đoạn 1–4

### Giai đoạn 1 — Thiết kế

Mục tiêu: thống nhất thiết kế chung trước khi code. Mỗi người nộp một tệp thiết kế trong `docs/Giai Doan 1/`.

- Chốt phương án: C++17 console; 3 khoa; 3 mức ưu tiên; mỗi phòng có FIFO (danh sách liên kết) và heap (mảng động); chọn phòng ít người chờ nhất; không ngắt ca đang khám; nhận bệnh phút 0–479 rồi khám hết người còn lại; thời lượng khám 5–15 phút. Chưa làm bỏ về và aging.
- Chốt định dạng tệp dữ liệu (`|` phân cách), menu, quy tắc kiểm tra đầu vào, và ví dụ tính tay (3 người cho kết quả chờ 0/9/3 khi bật ưu tiên, 0/4/8 khi tắt).
- Dựng bộ header trong `include/`, khung `.cpp` và `data/format_example.txt` (5 bệnh nhân); kiểm tra cú pháp C++17.

### Giai đoạn 2 — Cài đặt cấu trúc dữ liệu

Mục tiêu: chuyển thiết kế thành mã chạy được, kiểm tra từng mô-đun riêng. Bàn giao nằm trong `docs/Giai Doan 2/`.

| Người | Đã cài | Kiểm tra |
| --- | --- | --- |
| Leader | `Patient` (phân loại ưu tiên, kiểm tra đầu vào, thời gian chờ), `SimulationConfig`, `Simulation` (`initialize`, `admit`, `step`, `isFinished`), `main`, `Makefile` | `make test-leader-isolated` 8/8; kiểm tra tràn số, lỗi cấp phát, chạy lại 4 cấu hình không cộng dồn |
| A | `LinkedQueue` (FIFO O(1)), `ClinicRoom` (FIFO + heap, gọi khám) | `make test-a`, `make test-a-queue` |
| B | `PriorityHeap` (min-heap mảng động), `Statistics` (chờ trung bình/lớn nhất theo mức, hàng chờ lớn nhất), Merge Sort tự cài | `make test-b` |
| C | `PatientStore` (sở hữu bệnh nhân), `FileIO`, `DataGenerator` (sinh có seed), `ConsoleUI` (menu) | `make test-c`, 0 leak |

Phát hiện lúc tích hợp: `test-leader` với mô-đun thật ban đầu lỗi liên kết vì thiếu phần của B; sau khi B merge thì đạt. AddressSanitizer của Apple clang trên macOS 26 không chạy được nên dùng `leaks` và UBSan thay thế.

### Giai đoạn 3 — Chức năng cơ bản

Mục tiêu: chương trình console dùng được từ menu: tiếp nhận, cấp số, phân phòng, gọi khám, lưu và mở lại. Bàn giao nằm trong `docs/Giai Doan 3/`.

- **Leader:** tiếp nhận ở menu 4 tự cấp ticket, phút đến bằng phút hiện tại và xếp phòng ngay (hàm `assignArrivals`); thêm `test-all`, `test-console`, `test-asan` vào Makefile; viết script `tests/console/run_console_tests.sh` và các ca `leader_*` (3 người hai chế độ, hòa phòng, cấp cứu khi phòng bận, bộ mẫu 4 cấu hình, so sánh 3 kịch bản). Rà lại và sửa các ca của A.
- **A:** snapshot phòng (menu 5 và 7), theo dõi 20 phút đầu của bộ mẫu có bảng tính tay, ca tắt ưu tiên, ảnh console trong `docs/Giai Doan 3/img/`. Không phát hiện lỗi ở `LinkedQueue`/`ClinicRoom`.
- **B:** nối danh sách chờ sắp xếp Merge Sort vào menu 7; mức ưu tiên chưa có ca khám hiện "Chua co du lieu" thay vì chia cho 0; kiểm tra `reset()` khi chạy lại; kịch bản `b_*` và dữ liệu `tests/data/`.
- **C:** nạp `data/patients_working.txt` khi khởi động; tiếp nhận cho nhập lại khi bị từ chối; bộ mẫu `data/patients_sample.txt` (30 bản ghi); `tests/data/invalid_mix.txt`; kịch bản `c_*`; điều chỉnh Makefile cho MinGW.

### Giai đoạn 4 — Mô phỏng trọn ngày và so sánh

Mục tiêu: chạy trọn một ngày, ghi trạng thái từng phút, so sánh 4 kịch bản trên cùng dữ liệu. Bàn giao nằm trong `docs/Giai Doan 4/`.

- **Leader:** `Simulation::lastFinishMinute()` cho phút kết thúc ngày; test bất biến trọn ngày 180 người × 4 cấu hình (đủ người, không khám trước khi đến, không chồng ca, đúng khoa, đúng thứ tự); giải thích bằng số liệu vì sao 2 phòng bật/tắt ưu tiên có lúc trùng kết quả (trùng khi và chỉ khi không ai được gọi vượt người đến trước); nháp mục Mô phỏng và bảng độ phức tạp.
- **A:** test không ngắt ca, đối chiếu 60 phút đầu của nhật ký với tính tay, đối chiếu `maxWaiting` từ nhật ký, sơ đồ một phút và ảnh menu 9/nhật ký trong `docs/Giai Doan 4/img/`.
- **B:** hàm chờ TB chung, tính tay 4 kịch bản trên bộ 5 người, bảng và nhận xét 4 kịch bản. *Phần này từng bị commit `5c0fb23` ghi đè; đã khôi phục từ commit `1d7f762` ngày 26/11.*
- **C:** `data/patients_day.txt` (180 người, seed 2026), nhật ký từng phút ra tệp ở menu 6, bảng gộp menu 9 và `results/comparison.txt`, trả phiên về cấu hình cũ sau so sánh.

Kết quả chính (`results/comparison.txt`, 180 người):

| Kịch bản | Chờ TB mức 0 / 1 / 2 | Chờ TB chung | Phút kết thúc |
| --- | --- | --- | --- |
| 1 phòng, bật ưu tiên | 5.5 / 18.49 / 168.16 | 77.75 | 723 |
| 1 phòng, tắt ưu tiên | 74.45 / 80.03 / 74.97 | 77.36 | 723 |
| 2 phòng, bật ưu tiên | 2.55 / 4.30 / 4.03 | 3.99 | 492 |
| 2 phòng, tắt ưu tiên | 2.55 / 5.03 / 3.34 | 4.07 | 492 |

Ưu tiên chuyển thời gian chờ từ mức 0/1 sang mức 2 mà gần như không đổi chờ TB chung; thêm phòng giảm chờ TB chung khoảng 95%. Với 2 phòng/khoa, hai chế độ chỉ khác nhau ở 5 lượt gọi vượt.

### Giai đoạn 5 — Kiểm thử và thực nghiệm (đang làm)

- Mốc 0: khôi phục phần G4 của B (đã xong); chạy Windows và ASan (C); kiểm tra chéo ca `b_*` (A).
- Bộ `data/patients_large.txt` 10.000 bản ghi; `make bench` đo thời gian mô phỏng, Merge Sort, nạp/lưu với N = 1.000–20.000 trên ít nhất 2 máy; đo bộ nhớ đỉnh và rò rỉ trên bộ lớn.
- Bảng kiểm thử tổng hợp, biểu đồ và nhận xét đối chiếu độ phức tạp. Chi tiết: [tổng quan](docs/11_Tong_quan_giai_doan_5.md), [phân công](docs/12_Phan_cong_giai_doan_5.md).

Giai đoạn 6 (báo cáo, slide, bộ nộp cuối) chưa làm.

## Giải thích menu console theo giai đoạn

Menu hiện không dấu để chạy đúng trên mọi console. Dùng theo thứ tự `1` (hoặc `2`) → `3` → `5`/`6` → `7` → `9`.

| Mục | Ý nghĩa | Giai đoạn hoàn thiện |
| --- | --- | --- |
| 1 | Nạp bệnh nhân từ tệp; dòng sai định dạng bị từ chối và báo lỗi | GĐ2 (`FileIO`, `PatientStore`), GĐ3 (nạp tự động `patients_working.txt`) |
| 2 | Sinh dữ liệu ngẫu nhiên theo seed (cùng seed cho cùng kết quả) | GĐ2 (`DataGenerator`) |
| 3 | Chọn số phòng mỗi khoa (1 hoặc 2), bật/tắt ưu tiên và khởi tạo; bắt buộc trước khi chạy, chạy lại thì reset | GĐ2 (`Simulation::initialize`) |
| 4 | Tiếp nhận thêm một bệnh nhân: tự cấp số thứ tự, phút đến = phút hiện tại, xếp phòng ngay; bị từ chối thì nhập lại | GĐ3 |
| 5 | Chạy đúng 1 phút rồi in trạng thái từng phòng | GĐ3 |
| 6 | Chạy đến hết; hỏi có in từng phút không (y/n) và có ghi nhật ký từng phút ra tệp không (mặc định `results/log.txt`) | GĐ3; GĐ4 thêm nhật ký và phút kết thúc ngày |
| 7 | Xem phòng và thống kê chờ (trung bình/lớn nhất theo mức, chờ TB chung, hàng chờ lớn nhất, danh sách chờ giảm dần bằng Merge Sort) | GĐ2 (`Statistics`), GĐ3 (nối vào menu) |
| 8 | Lưu dữ liệu đầu vào hoặc kết quả (kết quả vào `results/`) | GĐ3 |
| 9 | So sánh 4 kịch bản (1/2 phòng × có/không ưu tiên) trên cùng dữ liệu; in bảng gộp, lưu `results/comparison.txt`, rồi trả phiên về cấu hình cũ | GĐ4 |
| 0 | Lưu đầu vào vào `data/patients_working.txt` rồi thoát; mở lại là phiên mới | GĐ3 |

**Mỗi phút mô phỏng:** hoàn tất ca khám → tiếp nhận người đến → gọi người tiếp theo vào phòng rảnh. Sau phút 479 không nhận thêm nhưng vẫn khám hết người còn chờ.

**Cách đọc kết quả:**

```text
Phut 0
Phong 1 (Noi): cho thuong=1, cho uu tien=0, dang kham=BN01, ket thuc=5
```

- `cho thuong`: số người trong hàng FIFO (danh sách liên kết); `cho uu tien`: số người trong heap.
- `dang kham`: mã bệnh nhân đang khám; `ket thuc`: phút ca khám xong.
- Thống kê `Muc 0/1/2: bat dau, cho TB, cho max`: số người đã bắt đầu khám, thời gian chờ trung bình và lớn nhất (phút) của từng mức ưu tiên; mức chưa có ca khám hiện "Chua co du lieu".
- Nhật ký (menu 6) thêm dòng sự kiện mỗi phút: `Hoan tat: <mã> (phong p)` và `Goi kham: <mã> (phong p, cho w)`.

**Theo từng giai đoạn:**

- **GĐ1 (thiết kế):** chỉ chốt danh sách mục menu, định dạng tệp và quy tắc nhập; chưa chạy được.
- **GĐ2 (cấu trúc dữ liệu):** từng mô-đun (FIFO, heap, thống kê, kho, tệp, menu) được kiểm thử riêng; menu chưa nối đủ vào mô phỏng.
- **GĐ3 (chức năng cơ bản):** menu 1–8 và 0 chạy được với mô-đun thật; kiểm thử bằng kịch bản `tests/console/*.in` (`make test-console`).
- **GĐ4 (mô phỏng và thống kê):** mô phỏng trọn ngày, nhật ký ra tệp, mục 9 so sánh 4 kịch bản.
- **GĐ5 (đang làm):** bộ 10.000 bản ghi, đo thời gian và bộ nhớ, ASan và Windows.
- **GĐ6 (chưa làm):** báo cáo, slide, bộ nộp cuối.

## Kế hoạch và tài liệu thiết kế

- [Tổng quan 6 giai đoạn](01_Tong_quan_6_giai_doan.md)
- Giai đoạn 1: [tổng quan](docs/02_Tong_quan_giai_doan_1.md), [phân công](docs/03_Phan_cong_giai_doan_1.md); thiết kế [Leader](docs/Giai%20Doan%201/G1_Leader_Thiet_ke.md), [A](docs/Giai%20Doan%201/G1_A_Thiet_ke.md), [B](docs/Giai%20Doan%201/G1_B_Thiet_ke.md), [C](docs/Giai%20Doan%201/G1_C_Thiet_ke.md)
- Giai đoạn 2: [tổng quan](docs/04_Tong_quan_giai_doan_2.md), [phân công](docs/05_Phan_cong_giai_doan_2.md), bàn giao trong `docs/Giai Doan 2/`
- Giai đoạn 3: [tổng quan](docs/06_Tong_quan_giai_doan_3.md), [phân công](docs/07_Phan_cong_giai_doan_3.md), bàn giao trong `docs/Giai Doan 3/`
- Giai đoạn 4: [tổng quan](docs/08_Tong_quan_giai_doan_4.md), [phân công](docs/09_Phan_cong_giai_doan_4.md), bàn giao trong `docs/Giai Doan 4/`, [nháp Mô phỏng](docs/Giai%20Doan%204/G4_Leader_Mo_phong_nhap.md)
- Giai đoạn 5: [tổng quan](docs/11_Tong_quan_giai_doan_5.md), [phân công](docs/12_Phan_cong_giai_doan_5.md)
- [Quy trình Git cho thành viên](MEMBER_GIT_WORKFLOW.md)
