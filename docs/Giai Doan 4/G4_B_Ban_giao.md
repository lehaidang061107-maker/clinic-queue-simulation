# Bàn giao Giai đoạn 4 — Thành viên B

**Phụ trách:** `include/Statistics.h`, `src/Statistics.cpp`, `tests/test_b.cpp`, kịch bản nhập `tests/console/b_*.in`, đầu ra mẫu `tests/console/b_*.expected`, thực nghiệm so sánh `results/comparison.txt` và bảng nhận xét 4 kịch bản.

## 1. Đã làm gì

- **Hoàn thiện nợ Giai đoạn 3 (Mốc 0 phân công G4):**
  - Sửa `Statistics::reset(int roomCount)` trong `src/Statistics.cpp`: cấp phát mảng động `new RoomStats[roomCount]` trước khi xóa mảng cũ và đặt lại toàn bộ bộ đếm, đảm bảo cam kết an toàn ngoại lệ mạnh (strong exception safety) khi `Simulation::initialize` gọi `reset`.
  - Bọc mảng con trỏ bằng `std::unique_ptr` trong `makeSortedWaitReport` để tránh rò rỉ bộ nhớ nếu cấp phát mảng tạm thất bại.
  - Chạy kiểm tra chéo các kịch bản `tests/console/a_*.in` của Thành viên A trên Windows (MinGW-w64) và cập nhật kết quả vào `docs/Giai Doan 3/G3_A_Ban_giao.md`.
- **Bổ sung hàm tính thời gian chờ trung bình chung toàn hệ thống (Mốc 1 & Mục 4.1 phân công G4):**
  - Bổ sung vào `include/Statistics.h` và `src/Statistics.cpp` các phương thức:
    - `totalStartedCount() const`: tổng số bệnh nhân đã bắt đầu khám của cả 3 mức ưu tiên.
    - `totalWaitingTime() const`: tổng thời gian chờ của cả 3 mức ưu tiên.
    - `averageWait() const`: trả về `totalWaitingTime() / totalStartedCount()` (trả về `0.0` khi `totalStartedCount() == 0`, không chia cho `0`).
    - `formatAverageWait() const`: trả về chuỗi `"Chua co du lieu"` khi chưa có bệnh nhân nào bắt đầu khám, ngược lại trả về chuỗi giá trị của `averageWait()`.
  - Đã chốt tên hàm với Leader và Thành viên C trước khi C tích hợp vào bảng Menu `9` và `saveComparisonResults` trong `src/FileIO.cpp`.
- **Mở rộng kiểm thử đơn vị và kịch bản console (`tests/test_b.cpp`, `tests/console/b_*.in`, `tests/console/b_*.expected`):**
  - Bổ sung hàm `checkSystemAverageWait()` trong `tests/test_b.cpp` kiểm thử 3 trường hợp: bộ không có ai (`0` người), bộ `3` người đủ 3 mức ưu tiên, và bộ thiếu một mức ưu tiên (không có mức `1`), kèm kiểm tra sau `reset()`.
  - Tạo 2 kịch bản kiểm thử console tự chứa (lưu tệp làm việc vào `build/console/*_working.txt`, không ghi đè `data/patients_working.txt`):
    - `tests/console/b_empty_avg.in` (và `.expected`): kiểm tra kho rỗng hiển thị `"Chua co du lieu"`, không chia cho `0`.
    - `tests/console/b_format_example_2.in` (và `.expected`): chạy Menu `9` trên `tests/data/format_example_2.txt` để đối chiếu số liệu 4 kịch bản với bảng tính tay.
- **Tính tay đối chiếu và chạy thực nghiệm so sánh (`results/comparison.txt`):**
  - Tính tay đầy đủ 4 kịch bản trên `tests/data/format_example_2.txt` (`5` bệnh nhân) và đối chiếu khớp với đầu ra Menu `9`.
  - Chạy thực nghiệm chính thức 4 kịch bản trên tập dữ liệu trọn ngày `data/patients_day.txt` (`180` bệnh nhân, giờ đóng tiếp nhận `480`, thời lượng `5–15` phút, seed `2026`), xuất kết quả ra `results/comparison.txt` và lập bảng nhận xét.

## 2. Cách chạy

Môi trường: Windows, `g++` (MinGW-w64, C++17). Ngày chạy: 2026-11-25.

```powershell
# Biên dịch toàn bộ chương trình và bộ test đơn vị của B
New-Item -ItemType Directory -Force -Path build, build\console, results | Out-Null
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/*.cpp -o build/clinic_simulator.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_b.cpp src/PriorityHeap.cpp src/Statistics.cpp src/PatientStore.cpp src/Patient.cpp -o build/test_b.exe
.\build\test_b.exe

# Chạy 2 kịch bản kiểm thử console Giai đoạn 4 của B (tệp working lưu riêng vào build\console\)
cmd /c ".\build\clinic_simulator.exe < tests\console\b_empty_avg.in > build\console\b_empty_avg.out"
cmd /c ".\build\clinic_simulator.exe < tests\console\b_format_example_2.in > build\console\b_format_example_2.out"
```

## 3. Kết quả kiểm tra

| Ca | Kịch bản nhập | Mong đợi | Thực tế | Trạng thái |
|---|---|---|---|---|
| Test đơn vị G4 (`test_b.exe`) | `tests/test_b.cpp` | 4 nhóm test `PriorityHeap`, `Statistics`, `SystemAverageWait`, `MergeSort` PASS, 0 cảnh báo | 4/4 nhóm PASS, không lỗi/cảnh báo | Đạt |
| Kho rỗng không chia cho 0 | `tests/console/b_empty_avg.in` | Mức `0/1/2` và chờ TB chung đều hiện `"Chua co du lieu"`, lưu `build/console/b_empty_avg_working.txt` | Hiển thị đúng `"Chua co du lieu"`, khớp `.expected` | Đạt |
| So sánh 4 kịch bản trên bộ 5 người | `tests/console/b_format_example_2.in` | Menu `9` chạy 4 cấu hình trên `tests/data/format_example_2.txt`: 1 phòng bật ưu tiên TB chung `8.4` (`2` / `10.5` / `9.5`), 1 phòng tắt ưu tiên TB chung `8.4` (`17` / `10.5` / `2`), 2 phòng bật ưu tiên TB chung `3.2` (`4` / `1.5` / `4.5`), 2 phòng tắt ưu tiên TB chung `3.2` (`4` / `4` / `2`) | Khớp 100% số liệu tính tay và thứ tự xếp hạng ở cả 4 kịch bản | Đạt |
| Thực nghiệm trọn ngày (`180` người) | Nạp `data/patients_day.txt` → Menu `9` | Xuất đầy đủ 4 kịch bản ra `results/comparison.txt`, khớp số liệu trên màn hình console | Xuất thành công `results/comparison.txt`, số liệu console và tệp khớp 100% | Đạt |

**Đoạn đầu ra trích xuất từ kiểm thử thực tế:**

=== Ket qua b_empty_avg.out (kho rong, Menu 7) ===
Thong ke (tam thoi)
Muc 0: Chua co du lieu
Muc 1: Chua co du lieu
Muc 2: Chua co du lieu
Cho TB chung: Chua co du lieu
Da hoan tat=0, cho he thong lon nhat=0
Phong 1: cho lon nhat=0
Phong 2: cho lon nhat=0
Phong 3: cho lon nhat=0

=== Ket qua b_format_example_2.out (4 kich ban tren 5 benh nhan) ===
--- 1 phong, bat uu tien ---
Muc 0: bat dau=1, cho TB=2, cho max=2
Muc 1: bat dau=2, cho TB=10.5, cho max=13
Muc 2: bat dau=2, cho TB=9.5, cho max=19
Cho TB chung: 8.4, Da hoan tat=5, cho he thong lon nhat=4, phut ket thuc=25
--- 1 phong, tat uu tien ---
Muc 0: bat dau=1, cho TB=17, cho max=17
Muc 1: bat dau=2, cho TB=10.5, cho max=13
Muc 2: bat dau=2, cho TB=2, cho max=4
Cho TB chung: 8.4, Da hoan tat=5, cho he thong lon nhat=4, phut ket thuc=25
--- 2 phong, bat uu tien ---
Muc 0: bat dau=1, cho TB=4, cho max=4
Muc 1: bat dau=2, cho TB=1.5, cho max=3
Muc 2: bat dau=2, cho TB=4.5, cho max=9
Cho TB chung: 3.2, Da hoan tat=5, cho he thong lon nhat=3, phut ket thuc=15
--- 2 phong, tat uu tien ---
Muc 0: bat dau=1, cho TB=4, cho max=4
Muc 1: bat dau=2, cho TB=4, cho max=8
Muc 2: bat dau=2, cho TB=2, cho max=4
Cho TB chung: 3.2, Da hoan tat=5, cho he thong lon nhat=3, phut ket thuc=15

=== Ket qua thuc nghiem data/patients_day.txt (180 benh nhan, seed 2026) ===
Phan bo dau vao: Muc 0=20 ca, Muc 1=87 ca, Muc 2=73 ca | Khoa Noi=59 (552p), Khoa Nhi=51 (517p), Khoa Ngoai=70 (697p)
--- 1 phong/khoa, bat uu tien ---
Muc 0: bat dau=20, cho TB=5.5, cho max=13
Muc 1: bat dau=87, cho TB=18.4943, cho max=50
Muc 2: bat dau=73, cho TB=168.164, cho max=408
Cho TB chung=77.75 (13995/180), Da hoan tat=180, cho he thong lon nhat=41 (P1=10, P2=9, P3=26), phut ket thuc=723
--- 1 phong/khoa, tat uu tien ---
Muc 0: bat dau=20, cho TB=74.45, cho max=220
Muc 1: bat dau=87, cho TB=80.0345, cho max=241
Muc 2: bat dau=73, cho TB=74.9726, cho max=231
Cho TB chung=77.3611 (13925/180), Da hoan tat=180, cho he thong lon nhat=43 (P1=11, P2=11, P3=27), phut ket thuc=723
--- 2 phong/khoa, bat uu tien ---
Muc 0: bat dau=20, cho TB=2.55, cho max=8
Muc 1: bat dau=87, cho TB=4.29885, cho max=19
Muc 2: bat dau=73, cho TB=4.0274, cho max=32
Cho TB chung=3.99444 (719/180), Da hoan tat=180, cho he thong lon nhat=7 (P1=2, P2=1, P3=2, P4=1, P5=2, P6=2), phut ket thuc=492
--- 2 phong/khoa, tat uu tien ---
Muc 0: bat dau=20, cho TB=2.55, cho max=8
Muc 1: bat dau=87, cho TB=5.03448, cho max=22
Muc 2: bat dau=73, cho TB=3.34247, cho max=16
Cho TB chung=4.07222 (733/180), Da hoan tat=180, cho he thong lon nhat=7 (P1=2, P2=1, P3=2, P4=1, P5=2, P6=2), phut ket thuc=492

**Bảng tổng hợp so sánh 4 kịch bản (`results/comparison.txt`):**

| Kịch bản | Mức 0 Cấp cứu (TB / Max) | Mức 1 Ưu tiên (TB / Max) | Mức 2 Thường (TB / Max) | Chờ TB chung | Chờ hệ thống Max | Phút kết thúc |
|---|---|---|---|---|---|---|
| 1 phòng/khoa, bật ưu tiên | `5.5` / `13` | `18.4943` / `50` | `168.164` / `408` | `77.75` | `41` | `723` |
| 1 phòng/khoa, tắt ưu tiên | `74.45` / `220` | `80.0345` / `241` | `74.9726` / `231` | `77.3611` | `43` | `723` |
| 2 phòng/khoa, bật ưu tiên | `2.55` / `8` | `4.29885` / `19` | `4.0274` / `32` | `3.99444` | `7` | `492` |
| 2 phòng/khoa, tắt ưu tiên | `2.55` / `8` | `5.03448` / `22` | `3.34247` / `16` | `4.07222` | `7` | `492` |

**Nhận xét số liệu thực nghiệm:**

- **Giả định phân bố dữ liệu đầu vào:** Bộ sinh `DataGenerator` (seed `2026`) sinh tuổi đều trong `[0, 120]` (`121` giá trị) và xác suất cấp cứu `10%` (`1/10`). Trên mẫu `180` bệnh nhân, nhóm cấp cứu (Mức `0`) có `20` ca (`11.11%`), nhóm ưu tiên theo tuổi `< 6` hoặc `>= 60` (Mức `1`) có `87` ca (`48.33%`), và nhóm thông thường (Mức `2`) có `73` ca (`40.56%`). Riêng khoa Ngoại nhận `70` ca với tổng thời lượng `697` phút vượt quá `480` phút đóng tiếp nhận nên tạo điểm nghẽn rõ nhất khi chỉ có `1` phòng/khoa.
- **Tác động của cơ chế ưu tiên lên từng mức (cấu hình 1 phòng/khoa):** Khi bật ưu tiên, thời gian chờ trung bình của Mức `0` giảm từ `74.45` xuống `5.5` phút (giảm `92.6%`, chờ tối đa giảm từ `220` xuống `13` phút) và Mức `1` giảm từ `80.0345` xuống `18.4943` phút (chờ tối đa giảm từ `241` xuống `50` phút) nhờ được gọi từ `PriorityHeap`. Đổi lại, nhóm Mức `2` bị nhường lượt nên chờ trung bình tăng từ `74.9726` lên `168.164` phút (chờ tối đa lên `408` phút tại khoa Ngoại), trong khi chờ trung bình chung toàn hệ thống gần như không đổi (`77.75` so với `77.3611` phút) và giờ kết thúc đều ở phút `723`.
- **Tác động của việc tăng số phòng khám (từ 1 phòng/khoa lên 2 phòng/khoa):** Khi tăng gấp đôi công suất lên `6` phòng (`2` phòng/khoa, tổng năng lực phục vụ `2880` phút vượt xa tổng thời lượng khám `1766` phút), thời gian chờ trung bình chung giảm `94.9%` (từ `77.75` xuống `3.99444` phút ở chế độ bật ưu tiên), đỉnh hàng chờ toàn hệ thống giảm từ `41`–`43` người xuống còn `7` người, và phút hoàn tất ca cuối rút ngắn từ phút `723` xuống phút `492` (chỉ trễ `12` phút sau giờ đóng cửa `480`).
- **Mức độ khác biệt giữa bật và tắt ưu tiên khi có 2 phòng/khoa:** Ở cấu hình `2` phòng/khoa, tải mỗi phòng thấp (đỉnh hàng chờ từng phòng chỉ từ `1` đến `2` người) nên hầu như không xảy ra tình trạng cùng một phòng vừa có ca chờ ở `PriorityHeap` vừa có ca chờ ở `LinkedQueue`. Vì vậy, số liệu hai chế độ bật và tắt ưu tiên gần như trùng khớp: Mức `0` giữ nguyên (`TB = 2.55`, `max = 8`), Mức `1` chỉ lệch `0.74` phút (`4.29885` so với `5.03448`), và chờ trung bình chung chỉ chênh `0.078` phút (`3.99444` so với `4.07222`).

## 4. Kiểm tra tích hợp và kiểm tra chéo

- `Statistics.h` và `Statistics.cpp` đã liên kết, biên dịch sạch và chạy trực tiếp cùng `PatientStore` và `FileIO` (Thành viên C), `Simulation` (Leader), `LinkedQueue` và `ClinicRoom` (Thành viên A) trong `clinic_simulator.exe`.
- **Kiểm tra chéo Giai đoạn 3 với Thành viên A:** Đã chạy toàn bộ kịch bản `tests/console/a_*.in` của A trên Windows (MinGW-w64) và ghi nhận kết quả đạt vào `docs/Giai Doan 3/G3_A_Ban_giao.md`.
- **Kiểm tra chéo Giai đoạn 4:**
  - B chạy các ca `tests/console/c_*.in` của Thành viên C trên Windows (MinGW-w64): cả `c_day_summary` và `c_compare_export` đều chạy khớp đầu ra mong đợi và xuất đúng định dạng tệp `results/comparison.txt`.
  - A chạy kiểm tra chéo các ca `tests/console/b_*.in` của B: Đã chạy đạt trên Windows (MSYS2 UCRT64): b_empty_avg và b_format_example_2 đều khớp .expected.

## 5. Phần còn lại

- **Phần của B:** Không còn lỗi biên dịch hoặc cảnh báo với cờ `-Wall -Wextra -Wpedantic`; đã kiểm tra `git diff --check` không có khoảng trắng thừa; các ca kiểm thử console của B đều xuất tệp tạm vào `build/console/*_working.txt`, không làm bẩn `data/patients_working.txt`.
- **Việc chờ thành viên khác:** Chờ Thành viên A chạy kiểm tra chéo 2 ca `tests/console/b_*.in` trên nhánh `develop` và Leader rà soát tổng hợp trước khi chốt thẻ kết thúc Giai đoạn 4.
- **Ghi chú chuẩn bị cho Giai đoạn 5 (mở rộng quy mô 10.000 bệnh nhân):** Hiện `Statistics` mới lưu các chỉ số tổng hợp nghiệp vụ hàng đợi (thời gian chờ trung bình/tối đa theo mức ưu tiên, chờ trung bình chung, đỉnh hàng chờ từng phòng và toàn hệ thống), chưa ghi nhận thời gian thực thi CPU (`std::chrono`). Sang Giai đoạn 5 cần bổ sung bộ đo thời gian chạy từng giai đoạn mô phỏng và sắp xếp báo cáo để đánh giá hiệu năng thực tế trên tập dữ liệu lớn `data/patients_large.txt` (`10.000` bệnh nhân).