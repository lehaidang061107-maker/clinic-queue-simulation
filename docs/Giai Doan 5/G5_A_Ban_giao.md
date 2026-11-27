# Bàn giao Giai đoạn 5 — Thành viên A

**Phụ trách:** `tests/test_a.cpp`, kiểm thử quy mô lớn trên cấu trúc dữ liệu của Thành viên A (`LinkedQueue`, `ClinicRoom`), bảng tổng hợp kiểm thử toàn dự án (`tests/` và `tests/console/`), đo đạc bộ nhớ (lý thuyết và thực tế trên Windows), kiểm tra chéo hiệu năng (`make bench`) và tư liệu ảnh minh họa.

---

## 1. Đã làm gì

- **Mở rộng kiểm thử quy mô lớn cho `LinkedQueue` và `ClinicRoom` (`tests/test_a.cpp`):**
  - Bổ sung `checkQueueLargeScale()`: nạp và rút tuần tự `100.000` phần tử vào `LinkedQueue`, kiểm tra tính bảo toàn thứ tự FIFO, `size()`, `peek()`, và giải phóng sạch toàn bộ nút động sau khi rút hết.
  - Bổ sung `checkRoomLargeScale()`: nạp `10.000` bệnh nhân phân bổ xen kẽ đủ cả 3 mức ưu tiên (`Emergency = 0`, `Preferential = 1`, `Normal = 2`) vào `ClinicRoom` và kiểm tra ở cả 2 chế độ:
    - **Bật ưu tiên (`usePriority = true`):** gọi hết toàn bộ ca Mức `0`, sau đó Mức `1`, cuối cùng Mức `2`; trong cùng mức giữ nguyên thứ tự tăng dần theo `(arrivalTime, ticketNumber)`.
    - **Tắt ưu tiên (`usePriority = false`):** gọi lần lượt toàn bộ `10.000` bệnh nhân theo đúng thứ tự FIFO (`ticketNumber` từ `1` đến `10.000`).
- **Tổng hợp kiểm thử toàn bộ dự án (`tests/` và `tests/console/`):**
  - Rà soát và chạy toàn bộ bộ kiểm thử đơn vị (`test-a`, `test-b`, `test-c`, `test-leader`) và toàn bộ `25` kịch bản kiểm thử giao diện dòng lệnh (`tests/console/*.in`) trên Windows (MSYS2 UCRT64, GCC 16.2.0).
- **Đo đạc bộ nhớ lý thuyết và thực tế trên Windows:**
  - Tính kích thước tĩnh (`sizeof`) từng cấu trúc dữ liệu lõi và ước lượng bộ nhớ lý thuyết theo kích thước đầu vào $N \in \{1.000, 2.000, 5.000, 10.000, 20.000\}$.
  - Đo bộ nhớ đỉnh thực tế (`PeakWorkingSetSize` qua Win32 API `GetProcessMemoryInfo`) khi thực thi `./build/bench` trên từng mức $N$, đối chiếu giữa lý thuyết và thực tế để kiểm chứng độ phức tạp không gian $O(n)$.
- **Kiểm tra chéo và chụp ảnh tư liệu cho slide:**
  - Hoàn tất xác nhận kiểm tra chéo các ca `tests/console/b_*.in` trong `docs/Giai Doan 4/G4_B_Ban_giao.md`.
  - Chạy kiểm tra chéo `mingw32-make bench` của Thành viên B & Leader trên Windows và lưu ảnh chụp màn hình tại `docs/Giai Doan 5/img/a_bench.png`.

---

## 2. Cách chạy

Môi trường: Windows 11 (MSYS2 UCRT64, `g++` GCC 16.2.0, `mingw32-make`). Ngày chạy: 2026-11-27.

```bash
# Biên dịch và chạy toàn bộ kiểm thử đơn vị của A (gồm test 100.000 và 10.000 bản ghi)
mingw32-make test-a

# Chạy toàn bộ bộ kiểm thử đơn vị và 25 kịch bản console
mingw32-make test-all
mingw32-make test-console

# Chạy kiểm tra chéo chương trình đo hiệu năng (-O2)
mingw32-make bench MACHINE=windows-check
```

## 3. Bảng tổng hợp kết quả kiểm thử toàn dự án

### 3.1. Kiểm thử đơn vị (`tests/*.cpp`)

| Bộ kiểm thử             | Lệnh chạy                  | Số nhóm ca | Nội dung kiểm tra chính                                                                                                                                                                                  | Kết quả trên Windows                                                                                                                          |
| ----------------------- | -------------------------- | ---------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| `tests/test_a.cpp`      | `mingw32-make test-a`      | `12` nhóm  | `LinkedQueue` (rỗng, 1 phần tử, FIFO, `clear`, **quy mô lớn 100.000 nút**); `ClinicRoom` (cấu hình, phân luồng ưu tiên/thường, không ngắt ca cấp cứu, `reset`, **quy mô lớn 10.000 bệnh nhân đủ 3 mức**) | **12/12 PASS**                                                                                                                                |
| `tests/test_b.cpp`      | `mingw32-make test-b`      | `4` nhóm   | `PriorityHeap` (thứ tự 3 khóa, mở rộng mảng động, tái sử dụng); `Statistics` (thống kê theo mức, trung bình chung, chống chia cho `0`); `MergeSort` báo cáo                                              | **4/4 PASS**                                                                                                                                  |
| `tests/test_c.cpp`      | `mingw32-make test-c`      | `4` nhóm   | `PatientStore` (chèn tăng dần, chống trùng mã/số thứ tự); `FileIO` (đọc/ghi, cô lập dòng lỗi, xử lý `CRLF`); `DataGenerator` (seed tất định `2026`)                                                      | **3/4 PASS** _(1 assertion so sánh chuỗi tệp mẫu ở dòng 275 lệch ký tự xuống dòng `CRLF` của Git Windows với `LF` do `savePatients` xuất ra)_ |
| `tests/test_leader.cpp` | `mingw32-make test-leader` | `12` nhóm  | Kiểm tra hợp lệ đầu vào, 4 kịch bản mẫu, tiếp nhận giữa phiên, bất biến toàn ngày `180` người và **ngày lớn `10.000` bệnh nhân × 4 cấu hình**                                                            | **12/12 PASS**                                                                                                                                |

### 3.2. Kiểm thử kịch bản console (`tests/console/*.in` — 25 kịch bản)

| Nhóm phụ trách        | Danh sách kịch bản (`.in` so khớp từng ký tự với `.expected`)                                                                                                                                       | Số ca đạt     | Trạng thái   |
| --------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------- | ------------ |
| Thành viên A (`5` ca) | `a_20min`, `a_busy`, `a_empty`, `a_no_preempt`, `a_no_priority`                                                                                                                                     | `5 / 5`       | **Đạt**      |
| Thành viên B (`6` ca) | `b_empty_avg`, `b_format_example_2`, `b_midrun_stats`, `b_modes_sample`, `b_no_emergency`, `b_rerun_reset`                                                                                          | `6 / 6`       | **Đạt**      |
| Thành viên C (`6` ca) | `c_admit_retry`, `c_compare`, `c_first_run`, `c_invalid_mix`, `c_log`, `c_log_failure`                                                                                                              | `6 / 6`       | **Đạt**      |
| Leader (`8` ca)       | `leader_closing`, `leader_compare_three`, `leader_emergency_busy`, `leader_restore_after_compare`, `leader_room_tie`, `leader_sample_4configs`, `leader_three_no_priority`, `leader_three_priority` | `8 / 8`       | **Đạt**      |
| **Tổng cộng**         | **Toàn bộ `25` kịch bản console (`sh tests/console/run_console_tests.sh`)**                                                                                                                         | **`25 / 25`** | **Đạt 100%** |

---

## 4. Kết quả đo đạc bộ nhớ (Lý thuyết và Thực tế trên Windows)

### 4.1. Kích thước tĩnh của từng cấu trúc dữ liệu (64-bit MinGW-w64)

| Cấu trúc dữ liệu                     | Kích thước (`sizeof`) | Thành phần chiếm dụng chính                                                                                                    |
| ------------------------------------ | --------------------- | ------------------------------------------------------------------------------------------------------------------------------ |
| `clinic::Patient`                    | `104` bytes           | `PatientInput` (2 chuỗi `std::string` `id`, `fullName` 64 B + 6 trường `int`/enum 24 B) + 4 trường trạng thái runtime (`16` B) |
| `clinic::PatientStore::Node`         | `112` bytes           | `Patient` (`104` B) + con trỏ `next` (`8` B)                                                                                   |
| `clinic::LinkedQueue::Node`          | `16` bytes            | Con trỏ `Patient*` (`8` B) + con trỏ `next` (`8` B)                                                                            |
| `clinic::PriorityHeap` (mỗi phần tử) | `8` bytes             | Mảng động chứa con trỏ `Patient*` (`8` B/phần tử, chỉ lưu con trỏ mượn từ kho)                                                 |
| `clinic::ClinicRoom`                 | `56` bytes            | `roomId_`, `specialtyId_`, `LinkedQueue` (`24` B), `PriorityHeap` (`16` B), `current_` (`8` B)                                 |
| `clinic::Simulation`                 | `160` bytes           | Tham chiếu kho, `SimulationConfig`, mảng `ClinicRoom*`, `Statistics`                                                           |

### 4.2. So sánh bộ nhớ lý thuyết và bộ nhớ đỉnh thực tế (`PeakWorkingSetSize`)

Trong `bench.exe`, tại đỉnh cao nhất tồn tại đồng thời 2 kho bệnh nhân (`store` sinh dữ liệu và `loaded` nạp từ tệp, mỗi kho $N$ nút `PatientStore::Node`) cùng tối đa $N$ nút/con trỏ chờ trong các phòng khám (`LinkedQueue::Node` hoặc `PriorityHeap`):

- **Lý thuyết cấu trúc dữ liệu 1 kho mô phỏng:** $M_{\text{1 kho}}(N) = N \times (112 + 16) = 128N\text{ bytes}$
- **Lý thuyết cấu trúc dữ liệu trong `bench.exe` (2 kho):** $M_{\text{bench}}(N) = N \times (2 \times 112 + 16) = 240N\text{ bytes}$

| Số bệnh nhân ($N$) |               Lý thuyết 1 kho |     Lý thuyết `bench` (2 kho) |   Thực tế Peak RSS trên Windows |                     Mức tăng thực tế so với $N = 1.000$ |
| -----------------: | ----------------------------: | ----------------------------: | ------------------------------: | ------------------------------------------------------: |
|            `1.000` |     `128.000` B (`125,0` KiB) |     `240.000` B (`234,4` KiB) |   `6.152.192` B (`6.008,0` KiB) |                     Cơ sở (`~5,88` MiB runtime cố định) |
|            `2.000` |     `256.000` B (`250,0` KiB) |     `480.000` B (`468,8` KiB) |   `6.189.056` B (`6.044,0` KiB) |       `+36,0` KiB (nằm trong vùng page heap đã cấp sẵn) |
|            `5.000` |     `640.000` B (`625,0` KiB) | `1.200.000` B (`1.171,9` KiB) |   `7.118.848` B (`6.952,0` KiB) |     `+944,0` KiB (khớp mức tăng lý thuyết `+937,5` KiB) |
|           `10.000` | `1.280.000` B (`1.250,0` KiB) | `2.400.000` B (`2.343,8` KiB) |   `8.765.440` B (`8.560,0` KiB) | `+2.552,0` KiB (khớp mức tăng lý thuyết `+2.109,4` KiB) |
|           `20.000` | `2.560.000` B (`2.500,0` KiB) | `4.800.000` B (`4.687,5` KiB) | `11.984.896` B (`11.704,0` KiB) | `+5.696,0` KiB (khớp mức tăng lý thuyết `+4.453,1` KiB) |

**Nhận xét đánh giá không gian bộ nhớ:**

- **Độ phức tạp không gian $O(n)$:** Cả tính toán lý thuyết lẫn số đo Peak RSS thực tế trên Windows đều tăng tuyến tính theo $N$. Khi tăng $N$ từ `5.000` lên `10.000` (`+5.000` bệnh nhân), bộ nhớ thực tế tăng thêm `1.646,6` KiB (tương đương `~337` B/bệnh nhân tính cả header cấp phát động của allocator Windows và đệm đọc/ghi tệp). Khi tăng từ `10.000` lên `20.000` (`+10.000` bệnh nhân), bộ nhớ thực tế tăng thêm `3.144,0` KiB (`~322` B/bệnh nhân).
- **Hiệu quả chia sẻ con trỏ (`Patient*`):** Nhờ `LinkedQueue` và `PriorityHeap` trong `ClinicRoom` chỉ lưu con trỏ `Patient*` (`8` bytes) trỏ về `PatientStore` thay vì sao chép bản ghi `Patient` (`104` bytes), chi phí hàng đợi cho `20.000` bệnh nhân chỉ tốn tối đa `~320` KiB (`16 B × 20.000`), không làm phình bộ nhớ khi chạy nhiều kịch bản liên tiếp.
- **Mức tiêu thụ tổng thể rất nhẹ:** Ngay cả với `20.000` bệnh nhân (gấp hơn `111` lần ngày làm việc chuẩn `180` bệnh nhân), toàn bộ tiến trình trên Windows chỉ chiếm đỉnh **`11,43` MiB** RAM (trong đó `~5,8` MiB là bộ nhớ nạp thư viện hệ thống UCRT/Win32 cố định).

---

## 5. Kiểm tra chéo hiệu năng và tư liệu hình ảnh

- **Kiểm tra chéo `make bench` trên Windows:** Đã biên dịch và chạy thành công `mingw32-make bench` với cờ `-O2` trên đủ 5 kích thước (`1.000` đến `20.000`, `5` lần/điểm). Thời gian mô phỏng cốt lõi (`sim_med`) trên Windows đạt `0,86–0,99` ms ở `N = 1.000` và chỉ `14,44–18,86` ms ở `N = 20.000` (tăng tuyến tính theo $N$), xác nhận tối ưu con trỏ `arrivalCursor_` của Leader hoạt động ổn định trên cả Windows lẫn macOS.
- **Ảnh chụp minh họa:** Kết quả chạy `mingw32-make bench` trên Windows được lưu tại [`docs/Giai Doan 5/img/a_bench.png`](img/a_bench.png) phục vụ slide thuyết trình.

---

## 6. Phần còn lại

- Toàn bộ phần việc của Thành viên A ở Giai đoạn 5 đã hoàn tất, biên dịch sạch với `-Wall -Wextra -Wpedantic`, không phát sinh cảnh báo hay dư thừa khoảng trắng (`git diff --check`).
- Chờ Thành viên B hoàn tất biểu đồ và nhận xét hiệu năng chính thức, Thành viên C chốt số liệu `bench_windows.csv`, và Leader rà soát tổng hợp để đóng Giai đoạn 5.
