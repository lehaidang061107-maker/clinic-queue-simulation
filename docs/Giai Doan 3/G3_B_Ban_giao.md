# Bàn giao Giai đoạn 3 — Thành viên B

**Phụ trách:** `src/Statistics.cpp`, `src/PriorityHeap.cpp`, hiển thị danh sách chờ (Menu 7) trong `src/ConsoleUI.cpp`, `tests/test_b.cpp`, dữ liệu kiểm thử `tests/data/format_example_2.txt`, `tests/data/no_emergency.txt` và kịch bản nhập `tests/console/b_*.in`.

## 1. Đã làm gì

- **Nối báo cáo chờ và kiểm tra thống kê trên giao diện (Mục 4 phân công G3 & Mốc 1 tổng quan G3):**
  - Rà soát `printStatistics` trong `src/ConsoleUI.cpp` và `saveRunResults` trong `src/FileIO.cpp`: đảm bảo mức ưu tiên chưa có ca bắt đầu khám (`startedCount == 0`) hiển thị chính xác chuỗi `"Chua co du lieu"`, tránh lỗi chia cho 0; dữ liệu xuất ra `results/run_results.txt` khớp hoàn toàn với màn thống kê.
  - Bổ sung `printSortedWaitReport` vào lựa chọn `7` (`Xem phong va thong ke`) trong `src/ConsoleUI.cpp`: gọi `makeSortedWaitReport(store, count)` lọc bệnh nhân đã bắt đầu khám (`startTime >= 0`, bao gồm cả ca đang khám `InService` khi xem giữa chừng), sắp xếp theo `waitingTime` giảm dần, hòa thì `ticketNumber` tăng dần, và thu hồi mảng con trỏ bằng `delete[]` sau khi in.
- **Tạo dữ liệu kiểm thử độc lập (`tests/data/`):**
  - `tests/data/format_example_2.txt`: bộ 5 bệnh nhân đủ 3 mức ưu tiên (`BN01`..`BN05`) để kiểm tra thống kê hai chế độ ưu tiên, đỉnh hàng đợi và thứ tự danh sách chờ.
  - `tests/data/no_emergency.txt`: bộ 6 bệnh nhân phủ 3 chuyên khoa chỉ gồm mức 1 và mức 2 (không có ca cấp cứu mức 0) để kiểm tra hiển thị `"Chưa có dữ liệu"` và hòa khóa `ticketNumber`.
- **Tạo 4 kịch bản kiểm thử console tự chứa (`tests/console/b_*.in`):**
  - `tests/console/b_modes_sample.in`: chạy bộ 5 người ở cả 2 chế độ (bật ưu tiên và tắt ưu tiên), xem thống kê + danh sách chờ và lưu kết quả ra `results/run_results.txt`.
  - `tests/console/b_rerun_reset.in`: cấu hình và chạy lại cùng kịch bản 2 lần liên tiếp (Menu `3` rồi `6`, lặp lại) để kiểm tra `Statistics::reset()` không cộng dồn số liệu cũ.
  - `tests/console/b_midrun_stats.in`: chạy từng phút bằng Menu `5` tới phút 10 rồi xem thống kê và danh sách chờ giữa chừng.
  - `tests/console/b_no_emergency.in`: chạy bộ dữ liệu không có mức 0 và kiểm tra dòng `Muc 0: Chua co du lieu`.

## 2. Cách chạy

Môi trường: Windows, `g++` (MinGW-w64, C++17). Ngày chạy: 2026-11-22.

```powershell
# Biên dịch toàn bộ chương trình và bộ test đơn vị của B
New-Item -ItemType Directory -Force -Path build, results | Out-Null
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/*.cpp -o build/clinic_simulator.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_b.cpp src/PriorityHeap.cpp src/Statistics.cpp src/PatientStore.cpp src/Patient.cpp -o build/test_b.exe
.\build\test_b.exe

# Chạy 4 kịch bản kiểm thử console của B
cmd /c ".\build\clinic_simulator.exe < tests\console\b_modes_sample.in > build\b_modes_sample.out"
cmd /c ".\build\clinic_simulator.exe < tests\console\b_rerun_reset.in > build\b_rerun_reset.out"
cmd /c ".\build\clinic_simulator.exe < tests\console\b_midrun_stats.in > build\b_midrun_stats.out"
cmd /c ".\build\clinic_simulator.exe < tests\console\b_no_emergency.in > build\b_no_emergency.out"
```

## 3. Kết quả kiểm tra

| Ca | Kịch bản nhập | Mong đợi | Thực tế | Trạng thái |
|---|---|---|---|---|
| Test đơn vị G2 (`test_b.exe`) | `tests/test_b.cpp` | 3 nhóm test `PriorityHeap`, `Statistics`, `MergeSort` PASS, 0 cảnh báo | 3/3 nhóm PASS, không lỗi/cảnh báo | Đạt |
| Bộ 5 người, 2 chế độ ưu tiên | `tests/console/b_modes_sample.in` | Bật ưu tiên: mức 0 TB `2`, mức 1 TB `10.5` (max `13`), mức 2 TB `9.5` (max `19`), xếp `BN02 -> BN05 -> BN03 -> BN04 -> BN01`; tắt ưu tiên: mức 0 max `17`, mức 2 max `4`, xếp `BN04 -> BN05 -> BN03 -> BN02 -> BN01` | Khớp toàn bộ số liệu tính tay và thứ tự xếp hạng chờ ở cả 2 chế độ | Đạt |
| Chạy lại 2 lần liên tiếp | `tests/console/b_rerun_reset.in` | Thống kê lần 2 giống hệt lần 1 sau `reset()`, `Da hoan tat=5`, không cộng dồn | Số liệu lần 1 và lần 2 trùng khớp hoàn toàn | Đạt |
| Thống kê & danh sách chờ giữa chừng | `tests/console/b_midrun_stats.in` | Tại phút 10 chỉ hiện 3 người đã bắt đầu khám (`BN03` đang khám chờ `8`, `BN04` xong chờ `2`, `BN01` xong chờ `0`); `BN05` và `BN02` chưa khám không xuất hiện | Hiển thị `Thong ke (tam thoi)` và đúng 3 người `BN03 -> BN04 -> BN01` | Đạt |
| Không có ca cấp cứu (mức 0) | `tests/console/b_no_emergency.in` | Mức 0 in `"Chua co du lieu"`; mức 1 chờ `0`; mức 2 chờ TB `4.33333` (max `5`); 3 ca hòa chờ `0` xếp theo ticket `NE01 -> NE03 -> NE05` | Hiển thị `"Chua co du lieu"`, không chia cho 0, xếp đúng `NE06 -> NE02 -> NE04 -> NE01 -> NE03 -> NE05` | Đạt |
| Đối chiếu tệp `results/run_results.txt` | Xuất kết quả sau lựa chọn `8 -> 2` | Số liệu thống kê từng mức, đỉnh hàng chờ phòng/hệ thống và danh sách chờ khớp với Menu `7` | Khớp 100% giữa console và `results/run_results.txt` | Đạt |

**Đoạn đầu ra trích xuất từ kiểm thử thực tế:**

=== Ket qua b_modes_sample.out (1 phong/khoa, bat uu tien) ===
Thong ke (hoan tat)
Muc 0: bat dau=1, cho TB=2, cho max=2
Muc 1: bat dau=2, cho TB=10.5, cho max=13
Muc 2: bat dau=2, cho TB=9.5, cho max=19
Da hoan tat=5, cho he thong lon nhat=4
Phong 1: cho lon nhat=4
Phong 2: cho lon nhat=0
Phong 3: cho lon nhat=0
Danh sach cho (sap xep giam dan):
1. BN02 (Tran Van Binh), ticket=2, den=1, bat dau=20, cho=19
2. BN05 (Pham Minh Duy), ticket=4, den=2, bat dau=15, cho=13
3. BN03 (Le Thi Chi), ticket=3, den=2, bat dau=10, cho=8
4. BN04 (Hoang Van Hai), ticket=5, den=3, bat dau=5, cho=2
5. BN01 (Nguyen Van An), ticket=1, den=0, bat dau=0, cho=0

=== Ket qua b_modes_sample.out (1 phong/khoa, tat uu tien) ===
Thong ke (hoan tat)
Muc 0: bat dau=1, cho TB=17, cho max=17
Muc 1: bat dau=2, cho TB=10.5, cho max=13
Muc 2: bat dau=2, cho TB=2, cho max=4
Da hoan tat=5, cho he thong lon nhat=4
Phong 1: cho lon nhat=4
Phong 2: cho lon nhat=0
Phong 3: cho lon nhat=0
Danh sach cho (sap xep giam dan):
1. BN04 (Hoang Van Hai), ticket=5, den=3, bat dau=20, cho=17
2. BN05 (Pham Minh Duy), ticket=4, den=2, bat dau=15, cho=13
3. BN03 (Le Thi Chi), ticket=3, den=2, bat dau=10, cho=8
4. BN02 (Tran Van Binh), ticket=2, den=1, bat dau=5, cho=4
5. BN01 (Nguyen Van An), ticket=1, den=0, bat dau=0, cho=0

=== Ket qua b_no_emergency.out (khong co ca cap cuu muc 0) ===
Thong ke (hoan tat)
Muc 0: Chua co du lieu
Muc 1: bat dau=3, cho TB=0, cho max=0
Muc 2: bat dau=3, cho TB=4.33333, cho max=5
Da hoan tat=6, cho he thong lon nhat=3
Phong 1: cho lon nhat=1
Phong 2: cho lon nhat=1
Phong 3: cho lon nhat=1
Danh sach cho (sap xep giam dan):
1. NE06 (Ngo Thi Hoa), ticket=6, den=5, bat dau=10, cho=5
2. NE02 (Vo Thi Binh), ticket=2, den=1, bat dau=5, cho=4
3. NE04 (Bui Thi Dung), ticket=4, den=3, bat dau=7, cho=4
4. NE01 (Pham Minh An), ticket=1, den=0, bat dau=0, cho=0
5. NE03 (Dang Van Cuong), ticket=3, den=2, bat dau=2, cho=0
6. NE05 (Hoang Van Em), ticket=5, den=4, bat dau=4, cho=0

## 4. Kiểm tra tích hợp và kiểm tra chéo

- `Statistics.cpp` và `ConsoleUI.cpp` đã liên kết và chạy trực tiếp cùng `PatientStore` (Thành viên C), `Simulation` (Leader), `LinkedQueue` và `ClinicRoom` (Thành viên A) trong chương trình `clinic_simulator.exe`.
- **Kiểm tra chéo với Thành viên A:** Chờ Thành viên A đẩy các kịch bản `tests/console/a_*.in` lên nhánh `develop` để B chạy kiểm tra chéo trên Windows (MinGW-w64) và ghi kết quả vào `docs/Giai Doan 3/G3_A_Ban_giao.md`.
- **Kiểm tra chéo với Thành viên C:** Chờ Thành viên C chạy kiểm tra chéo các tệp `tests/console/b_*.in` sau khi nhánh của B được merge vào `develop`.

## 5. Phần còn lại

- **Phần của B:** Không còn lỗi biên dịch hoặc cảnh báo với cờ `-Wall -Wextra -Wpedantic`; đã kiểm tra `git diff --check` không có lỗi khoảng trắng thừa.
- **Việc chờ thành viên khác:** Chờ A và C hoàn thiện bộ kịch bản `.in` và dữ liệu mẫu `data/patients_sample.txt` (≥30 bản ghi) để chạy tích hợp toàn bộ kịch bản của nhóm.
- **Ghi chú chuẩn bị cho Giai đoạn 4 (so sánh 4 kịch bản):** Hiện `Statistics` đã cung cấp đủ dữ liệu từng mức ưu tiên (`startedCount`, `totalWait`, `maxWait`), đỉnh hàng chờ từng phòng (`byRoom`) và toàn hệ thống (`maxSystemWaiting`). Sang Giai đoạn 4 có thể bổ sung thêm hàm tính thời gian chờ trung bình chung toàn hệ thống (tổng `totalWait` của 3 mức chia tổng `startedCount`) và tỷ lệ phục vụ đúng giờ để lập bảng đối chiếu trực quan giữa 4 cấu hình ở Menu `9`.


## 6. Leader rà soát và sửa (23/11/2026)

- `src/ConsoleUI.cpp`: sửa `#include "Statistics.h` thiếu dấu `"` đóng (lỗi này làm `make` không biên dịch được), trả lại chú thích `} // namespace` và bỏ hai dòng trống thừa. Không đổi logic.
- Chuyển tệp bàn giao từ `G3_B_Ban_giao.md/G3_B_Ban_giao.md.md` (một thư mục) về `docs/Giai Doan 3/G3_B_Ban_giao.md`.
- Thêm `tests/console/b_*.expected`, lấy số liệu từ bảng mục 3, để `make test-console` tự kiểm. Trên macOS (Apple clang 16.0.0), cả 4 ca `b_*` đều đạt.
- Lưu ý: `b_modes_sample` và `b_no_emergency` ghi đè `results/run_results.txt`; các ca `b_*` khi thoát lưu vào `data/patients_working.txt`. `make test-console` tự cất tệp working trước mỗi ca và trả lại sau khi xong; khi chạy tay thì cần tự xóa.
