# Bàn giao Giai đoạn 3 — Leader

**Phụ trách:** `src/Simulation.cpp`, `include/Simulation.h`, `src/main.cpp`, `Makefile`, `README.md`, `tests/test_leader.cpp`, `tests/console/leader_*`, `tests/console/run_console_tests.sh`, `tests/data/three_patients.txt`.

Nhánh `feature/g3-leader-integration` được tạo **từ nhánh `feature/c-phase3` (PR của C)**, vì các ca menu dùng phần nạp khi khởi động và vòng nhập lại khi tiếp nhận của C. Cần merge PR của C vào `develop` trước PR này.

## 1. Đã làm gì

**Mốc 0**

- `Makefile`: thêm `test-all` (gồm `test-a`, `test-b`, `test-c`, `test-leader`), `test-console` và `test-asan`.
- `make test-leader` với mô-đun thật đạt 8/8 nhóm, không còn lỗi liên kết như ở G2.

**Giai đoạn 3** (đánh số theo mục 2 trong phân công)

1. **Tiếp nhận xếp phòng ngay** (Tổng quan: tiếp nhận thủ công).
   - `Simulation::admit` vẫn tự cấp ticket = max + 1, phút đến = `nextMinute()` và tự phân loại mức ưu tiên.
   - Sau khi thêm vào kho, hàm xếp bệnh nhân vào phòng ngay qua hàm mới `assignArrivals(minute)`. Hàm này chính là bước "tiếp nhận người đến" trước đây nằm trong `step()`, nên `step()` cũng gọi lại nó.
   - Vì chạy cùng một hàm theo thứ tự kho `(arrival, ticket)`, người nạp từ tệp có cùng phút đến vẫn được xếp trước người tiếp nhận tay. Kết quả mô phỏng không đổi so với G2.
   - Xếp phòng trước bước hoàn tất ca của phút đó vẫn cho cùng kết quả, vì người đang khám không tính vào số chờ.
   - Nếu xếp phòng lỗi (cấp phát thất bại), phiên bị khóa và người dùng được yêu cầu khởi tạo lại, giống cách `step()` xử lý lỗi.
   - `ConsoleUI` (menu 4) giờ in `phong=<mã>` thay cho "chua phan". Đã sửa `c_admit_retry.expected` cho khớp: BN91 thuộc khoa Nhi nên vào phòng 2.
2. **Phân phòng 2 phòng/khoa:** kiểm bằng ca `leader_room_tie` và nhóm test mới `immediateAdmission`. Khi hòa số chờ thì chọn mã phòng nhỏ; người đang khám không tính vào số chờ.
3. **Ví dụ 3 người qua menu:** `leader_three_priority` và `leader_three_no_priority` cho kết quả chờ 0/9/3 và 0/4/8.
4. **Bộ mẫu 4 cấu hình:** `leader_sample_4configs` dùng menu 9. Ở cả 4 cấu hình, số bắt đầu = số hoàn tất = 30 và không ai bắt đầu trước phút đến.
5. **Build, ASan:** xem mục 3. ASan không chạy được trên máy này; đã thay bằng `leaks` và UBSan.
6. **README:** cập nhật mục Hiện trạng, Cách chạy (kịch bản console) và Biên dịch.

## 2. Cách chạy

Máy: macOS 26.5 (arm64), Apple clang 16.0.0 (clang-1600.0.26.6), GNU Make 3.81, Command Line Tools SDK 15.2. Ngày chạy: 23/11/2026.

```bash
make
make test-all
make test-leader-isolated
make test-console            # = sh tests/console/run_console_tests.sh
```

`run_console_tests.sh` hoạt động như sau:

- Trước mỗi ca, tạm cất `data/patients_working.txt` để ca bắt đầu từ kho rỗng; chạy xong thì trả lại tệp.
- Mỗi dòng trong `<ca>.expected` phải xuất hiện trong đầu ra, đúng thứ tự (so khớp chuỗi con).
- Nếu ca lưu kết quả vào `build/console/<ca>_results.txt`, tệp đó được nối vào cuối đầu ra để so luôn.

Các ca Leader khi thoát lưu đầu vào vào `build/console/…_working.txt`, không ghi đè `data/patients_working.txt`.

Kiểm tra ngược: thêm một dòng mong đợi sai vào `leader_room_tie.expected` thì script báo `FAIL … thieu: phong=3.`. Bản đầu của script có lỗi báo PASS sai, do dùng tên biến `exp` trùng hàm có sẵn của awk; đã sửa.

## 3. Kết quả

### Test đơn vị

| Lệnh | Mong đợi | Thực tế | Đạt |
|---|---|---|---|
| `make` | Không lỗi/cảnh báo | Không cảnh báo với `-Wall -Wextra -Wpedantic` | Đạt |
| `make test-all` | A/B/C/Leader PASS | A 9, B 3, C 4, Leader 8 dòng PASS | Đạt |
| `make test-leader-isolated` | 9 nhóm PASS | 9/9 (thêm `immediateAdmission`) | Đạt |
| `leaks --atExit -- ./build/test_leader` | 0 leak | `0 leaks for 0 total leaked bytes` | Đạt |
| `leaks --atExit -- ./build/clinic_simulator < tests/console/leader_sample_4configs.in` | 0 leak | `0 leaks for 0 total leaked bytes` | Đạt |
| test-leader và bộ mẫu build với `-fsanitize=undefined` | Không có `runtime error` | 0 dòng `runtime error` | Đạt |
| `make test-asan` | ASan không báo lỗi | **Không chạy được.** Runtime ASan báo `CHECK failed: sanitizer_malloc_mac.inc:189` trước khi vào `main`. Một chương trình rỗng `int main(){}` cũng bị lỗi này, nên nguyên nhân là toolchain (CLT SDK 15.2 trên macOS 26.5), không phải mã dự án | Chưa |

Test mới `immediateAdmission` trong `tests/test_leader.cpp`:

- Hai phòng Nội trống, tiếp nhận 2 người cùng phút: người 1 vào phòng 1, người 2 vào phòng 2.
- Khi cả hai phòng đang khám và hàng rỗng, người tiếp theo vào phòng 1.
- Người cấp cứu tiếp nhận ở phút 2 vào heap, không ngắt ca N1 (0–5). E1 bắt đầu lúc 5, còn N2 (đến phút 0) bắt đầu lúc 10.
- Người nạp từ tệp đến phút 1 được xếp trước người tiếp nhận tay cùng phút.

### Ca menu (`make test-console`: 9 ca đạt, gồm cả 3 ca của C)

| Ca | Mong đợi | Thực tế | Đạt |
|---|---|---|---|
| `leader_three_priority` | Chờ BN01/BN02/BN03 = 0/9/3 | `BN02\|2\|1\|10\|9`, `BN03\|3\|2\|5\|3`, `BN01\|1\|0\|0\|0`; Mức 1 chờ TB 3, Mức 2 chờ TB 4.5, max 9 | Đạt |
| `leader_three_no_priority` | 0/4/8 | `BN03\|3\|2\|10\|8`, `BN02\|2\|1\|5\|4`, `BN01\|1\|0\|0\|0` | Đạt |
| `leader_room_tie` | Người 1 vào phòng mã nhỏ, người 2 vào phòng còn lại | `ticket=1 … phong=1.`, `ticket=2 … phong=2.`; phút 0 cả hai bắt đầu khám | Đạt |
| `leader_emergency_busy` | Cấp cứu được gọi ngay sau ca hiện tại, trước người thường đang chờ | Phút 4: `cho thuong=1, cho uu tien=1, dang kham=N1`. Phút 5: `dang kham=E1, ket thuc=10`. Phút 10: `dang kham=N2`. Kết quả: `E1\|3\|2\|5\|3`, `N2\|2\|0\|10\|10` | Đạt |
| `leader_closing` | Phút 479 tiếp nhận được; phút 480 bị từ chối, vẫn khám hết | `phut den=479, phong=1.` → `Da dong tiep nhan.` → `dang kham=L1, ket thuc=484` → `Khong con phut nao can xu ly.`, `Da hoan tat=1` | Đạt |
| `leader_sample_4configs` | Nạp 30 người; ở 4 cấu hình: bắt đầu = hoàn tất = 30 | Mỗi cấu hình: Mức 0/1/2 bắt đầu 5/11/14, `Da hoan tat=30` | Đạt |

Số liệu bộ mẫu ở 4 cấu hình (chỉ ghi lại, chưa nhận xét so sánh; việc đó để giai đoạn 4):

| Cấu hình | Mức 0 chờ TB/max | Mức 1 chờ TB/max | Mức 2 chờ TB/max | Chờ hệ thống lớn nhất |
|---|---|---|---|---|
| 1 phòng, bật ưu tiên | 3.2 / 6 | 24.36 / 49 | 53 / 78 | 22 |
| 1 phòng, tắt ưu tiên | 41 / 72 | 32.45 / 71 | 33.29 / 75 | 23 |
| 2 phòng, bật ưu tiên | 2 / 6 | 10.64 / 22 | 20.43 / 40 | 18 |
| 2 phòng, tắt ưu tiên | 14.8 / 27 | 12 / 27 | 13.36 / 31 | 19 |

Kiểm tra thêm (chạy tay, không đưa vào bộ ca): chạy từng cấu hình bằng menu 3 → 6 → 8 (lưu kết quả). Cả 4 tệp kết quả đều có `Started: 30`, `Completed: 30`, đủ 30 dòng trong báo cáo chờ và không có dòng nào chờ âm.

## 4. Lỗi còn lại và việc chờ người khác

- **Windows:** Leader chưa tự build/chạy trên Windows. Bàn giao của C ghi `mingw32-make all test-c test-leader` và `test-a test-b` đạt trên MSYS2 GCC 16.2.0. Sau khi merge, người có máy Windows cần chạy lại `mingw32-make test-all` và các ca `leader_*` (cần `sh` của Git Bash/MSYS2 cho `test-console`).
- **ASan:** cần cập nhật Command Line Tools/Xcode cho khớp macOS 26.5, hoặc chạy `make test-asan` trên máy khác (Linux/WSL với g++ cũng được). Trong lúc chờ, đã dùng `leaks` và UBSan thay thế.
- **PR của B (`feature/b-giai-doan-3`) không biên dịch được:** trong `src/ConsoleUI.cpp` có dòng `#include "Statistics.h` thiếu dấu `"` đóng. B cần sửa trước khi merge. PR đó cũng sửa `ConsoleUI.cpp` (thêm danh sách chờ ở menu 7) nên sẽ xung đột nhẹ với PR của C.
- **Thứ tự merge đề xuất:** C (#17) → Leader (PR này) → A (#19, chỉ tài liệu/ảnh/kịch bản) → B (#18, sau khi sửa lỗi biên dịch).
- **Ca của A/B chưa có `.expected`:** script sẽ báo `SKIP`. A và B cần thêm `.expected` để `make test-console` tự kiểm được.
- **Kiểm tra chéo và rà toàn bộ `tests/console/`:** làm sau khi merge đủ các PR, theo mục 6 của phân công.
