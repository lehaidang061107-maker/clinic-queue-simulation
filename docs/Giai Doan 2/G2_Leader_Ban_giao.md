# Bàn giao Giai đoạn 2 — Leader

**Phụ trách:** `src/Patient.cpp`, `src/SimulationConfig.cpp`, `src/Simulation.cpp`, `src/main.cpp`, `Makefile`, `tests/test_leader.cpp`, `tests/support/leader_dependencies.cpp`.

## 1. Đã cài gì

- **Patient:** `classifyPriority` (cấp cứu → 0; dưới 6 hoặc từ 60 tuổi → 1; còn lại → 2); `validatePatientInput` kiểm tra mã/tên có nội dung, không chứa `|` hoặc xuống dòng, tuổi 0–120, khoa 1–3, ticket dương, phút đến trong `[0, closingTime)`, thời lượng dương, mức ưu tiên khớp tuổi/cấp cứu; `resetPatientState`; `waitingTime` (ném `logic_error` nếu chưa bắt đầu).
- **SimulationConfig:** `validateSimulationConfig` và `validateGeneratorConfig` kiểm tra giờ đóng, số phòng mỗi khoa, miền thời lượng, số bệnh nhân.
- **Simulation:**
  - `initialize`: kiểm tra cấu hình và toàn bộ input (kể cả tổng thời lượng có thể tràn `int`) **trước** khi đổi trạng thái; cấp phát phòng mới trước, rồi mới reset thống kê, xóa phòng cũ, reset trạng thái Patient. Lỗi cấp phát thì trả `false`, trạng thái cũ giữ nguyên.
  - `admit`: chỉ nhận khi đã khởi tạo và `nextMinute < closingTime`; tự cấp ticket = max + 1, arrival = `nextMinute`, mức ưu tiên tự phân loại; kiểm tra tràn ticket và tràn đồng hồ trước khi thêm kho.
  - `step`: hoàn tất ca đến giờ → xếp mọi người đến trong phút theo thứ tự kho `(arrival, ticket)` vào phòng đúng khoa ít người chờ nhất (hòa chọn mã phòng nhỏ) → quan sát hàng → gọi khám → quan sát lại. Nếu có ngoại lệ giữa phút, phiên bị khóa (`initialized_ = false`) để không cộng thống kê lặp khi thử lại.
  - `isFinished`: chỉ đúng khi đã qua giờ đóng nhận, mọi người Completed, không phòng nào bận/còn hàng.
- **main:** gọi `runConsole`; bắt ngoại lệ còn sót, in lỗi và trả mã 1.
- **Makefile:** `all`, `test-leader`, `test-leader-isolated`, `check-leader`; giai đoạn này thêm `test-a`, `test-a-queue` và sửa cờ linker cho macOS (`-dead_strip` thay `--gc-sections`).

## 2. Cách chạy

Môi trường: macOS 26.5 (arm64), Apple clang 16.0.0, GNU make. Ngày chạy: 2026-11-19.

```bash
make check-leader           # kiểm tra cú pháp phần Leader + main
make test-leader-isolated   # Leader với phần phụ thuộc giả (tests/support)
make test-leader            # Leader với mô-đun thật A/B/C
```

`tests/support/leader_dependencies.cpp` là **test double**, chỉ dùng cho `test-leader-isolated`, không bao giờ liên kết vào chương trình.

## 3. Kết quả kiểm tra

| Lệnh | Mong đợi | Thực tế |
|---|---|---|
| `make check-leader` | Không lỗi/cảnh báo | Đạt, không cảnh báo |
| `make test-leader-isolated` | 8 nhóm PASS | Đạt, 8/8 PASS (bên dưới) |
| `leaks --atExit -- ./build/test_leader_isolated` | 0 leak | 0 leaks for 0 total leaked bytes |
| `make test-leader` (mô-đun thật) | Liên kết và chạy PASS | **Chưa đạt: lỗi liên kết** do thiếu 7 symbol `PriorityHeap::*` và 9 symbol `Statistics::*` (B còn TODO). Không có symbol thiếu nào thuộc A hoặc C |

```text
PASS validators, priority boundaries, reset, integer bounds
PASS scenario rooms=1 priority=1 starts=0/10/5 waits=0/9/3
PASS scenario rooms=1 priority=0 starts=0/5/10 waits=0/4/8
PASS scenario rooms=2 priority=1 starts=0/5/2 waits=0/4/0
PASS scenario rooms=2 priority=0 starts=0/5/2 waits=0/4/0
PASS simultaneous arrivals, room ties, specialties, late completion, lifecycle
PASS admission 479/480, ticket gaps/overflow, duration budget, invalid input
PASS allocation failure propagation, disabled retry, recovery
```

Các ca trên phủ đủ ca bắt buộc giai đoạn 2: ví dụ 3 người hai chế độ (0/9/3 và 0/4/8), tuổi 5/6/59/60, khoa sai, hòa phòng, nhiều người cùng đến, cấp cứu không ngắt ca, nhận phút 479/480, hết giờ còn người chờ, chạy lại bốn cấu hình không cộng dồn.

## 4. Kiểm tra tích hợp với A và C

- Test A (`make test-a-queue`) và test C (`make test-c`) chạy đạt trên cùng máy, 0 leak. Chi tiết trong [G2_A](G2_A_Ban_giao.md) và [G2_C](G2_C_Ban_giao.md).
- Phần ClinicRoom của A đã được chạy thử với một heap tạm ngoài repo: ví dụ 3 người qua ClinicRoom thật cho đúng 0/9/3 và 0/4/8, khớp test Leader.

## 5. Lỗi còn lại và việc chờ

- **Chờ B:** `PriorityHeap.cpp`, `Statistics.cpp` (gồm `makeSortedWaitReport`). Khi có, chạy lại `make test-leader`, `make test-a`, `make` và cập nhật bảng mục 3.
- AddressSanitizer của Apple clang 16 lỗi khi khởi tạo trên macOS 26 (lỗi toolchain); dùng `leaks` thay thế. Cần chạy ASan trên Linux/Windows nếu có máy.
- Chưa build trên Windows/MinGW-w64.
- Các comment `TODO: Trien khai theo docs/03_Dac_ta_va_hop_dong_chung.md` trong tệp của B trỏ tới tài liệu không tồn tại; B xóa khi cài đặt.

## 6. Thời gian/bộ nhớ

Gọi `N` là số bệnh nhân, `R` là số phòng.

| Thao tác | Thời gian | Bộ nhớ phụ |
|---|---|---|
| `initialize` | O(N + R) | O(R) mảng phòng |
| `admit` | O(N) (tìm max ticket + `add` của kho) | O(1) ngoài node mới |
| `step` một phút | O(N + R·log k) — duyệt kho tìm người đến, chọn phòng O(R) mỗi người, heap O(log k) | O(1) |
| Chạy hết T phút | O(T·(N + R) + N·R + N log k) | O(1) |
| `isFinished` | O(N + R) | O(1) |

Duyệt toàn kho mỗi phút là điểm cần đo ở giai đoạn 5 với 10.000 bệnh nhân; chưa có số liệu đo.
