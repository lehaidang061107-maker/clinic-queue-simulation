# Bàn giao giai đoạn 4 — Thành viên A

## 1. Đã làm gì

### Mốc 0 (Hoàn tất nợ tồn Giai đoạn 3)

- Biên dịch và chạy kiểm thử `mingw32-make test-a` và `test-console` trên môi trường Windows (MSYS2 UCRT64, GCC 17), cập nhật kết quả thực tế vào `docs/Giai Doan 3/G3_A_Ban_giao.md`.
- Thực hiện kiểm tra chéo các ca console `c_*` của thành viên C trên Windows và ghi nhận kết quả vào `docs/Giai Doan 3/G3_C_Ban_giao.md`.
- Chụp bổ sung ảnh `docs/Giai Doan 3/img/a_tiep_nhan.png` hiển thị mã phòng được chỉ định khi tiếp nhận bệnh nhân (Menu 4).

### Giai đoạn 4 (Đối chiếu nhật ký, kiểm chứng phòng khám, sơ đồ & ảnh)

- **Kiểm chứng quy tắc không ngắt ca (Non-preemption):**
  - Bổ sung hàm kiểm thử đơn vị `checkRoomNoPreemption()` vào `tests/test_a.cpp` xác minh ca đang khám giữ phòng tới đúng `finishTime`, bệnh nhân cấp cứu mức `0` đến giữa ca phải chờ trong `PriorityHeap` và được gọi ngay tại phút ca hiện tại kết thúc.
  - Tạo ca kiểm thử console bắt buộc `tests/console/a_no_preempt.in` và `tests/console/a_no_preempt.expected` sử dụng `tests/data/three_patients.txt`.
  - Đối chiếu trực tiếp trên nhật ký trọn ngày `results/log_1phong_uutien.txt` (do C xuất từ `data/patients_day.txt`).
- **Đối chiếu nhật ký 60 phút đầu với tính tay:** Theo dõi chi tiết Phòng 3 (khoa Ngoại) từ phút `0` đến phút `60` trên `results/log_1phong_uutien.txt` và đối chiếu từng sự kiện với `data/patients_day.txt`.
- **Đối chiếu hàng chờ lớn nhất (`maxWaiting`):** Kiểm tra đỉnh hàng chờ từng phòng và toàn hệ thống từ nhật ký so với `RoomStats.maxWaiting` (Menu 8 `saveRunResults`) và `results/comparison.txt` ở cả cấu hình **1 phòng/khoa** và **2 phòng/khoa**.
- **Sơ đồ & hình ảnh cho báo cáo/slide:**
  - `docs/Giai Doan 4/img/a_so_do_1_phut.png`: Sơ đồ luồng xử lý trong 1 bước mô phỏng (`Simulation::step()`).
  - `docs/Giai Doan 4/img/a_menu9.png`: Ảnh kết quả bảng so sánh 4 kịch bản trên Menu 9.
  - `docs/Giai Doan 4/img/a_nhat_ky.png`: Ảnh đoạn nhật ký minh chứng không ngắt ca (phút 56–58).

---

## 2. Cách chạy

- **Môi trường:** Windows 11, MSYS2 UCRT64, `g++` (GCC 17), `mingw32-make`, `sh` (Git Bash).
- **Dữ liệu đối chiếu:** `data/patients_day.txt` (180 bệnh nhân, đóng nhận phút 480, thời lượng 5–15 phút, seed 2026), `results/log_1phong_uutien.txt`, `results/comparison.txt` (từ nhánh `feature/g4-member-c`, commit `9c026b7`).
- **Lệnh biên dịch và kiểm thử:**

  ```bash
  mingw32-make
  mingw32-make test-a
  sh tests/console/run_console_tests.sh

  ```

## 3. Kết quả

### 3.1. Kết quả kiểm thử đơn vị, ca console & kiểm tra chéo

| Hạng mục / Ca kiểm thử                                                          | Mong đợi                                                                                     | Thực tế                                                                                 | Kết quả |
| ------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------- | ------- |
| `mingw32-make test-a`                                                           | Toàn bộ test LinkedQueue, ClinicRoom, không ngắt ca và FileIO đạt                            | 3 nhóm test đạt (`PASS` toàn bộ, bao gồm `checkRoomNoPreemption`)                       | Đạt     |
| `a_no_preempt` (console)                                                        | Cấp cứu `BN03` đến phút 2 khi `BN01` đang khám; `BN03` chờ đến phút 5 mới được gọi (`cho 3`) | Xuất đúng chuỗi sự kiện `Hoan tat: BN01 (phong 1)` và `Goi kham: BN03 (phong 1, cho 3)` | Đạt     |
| Chạy chéo `test-b` (trên nhánh C/B)                                             | Toàn bộ unit test PriorityHeap, Statistics, MergeSort đạt trên Windows                       | 3 nhóm test đạt (`PASS` toàn bộ)                                                        | Đạt     |
| Chạy chéo `b_midrun_stats`, `b_modes_sample`, `b_no_emergency`, `b_rerun_reset` | Cả 4 ca console của B đạt trên Windows                                                       | `1 dat, 0 chua dat` cho cả 4 ca                                                         | Đạt     |

---

### 3.2. Đối chiếu nhật ký 60 phút đầu với tính tay — Phòng 3 (Khoa Ngoại)

Nguồn dữ liệu: `data/patients_day.txt` (các bệnh nhân khoa `3` đến trong 60 phút đầu) và `results/log_1phong_uutien.txt` (cấu hình 1 phòng/khoa, bật ưu tiên).

| Mốc phút  | Bệnh nhân mới đến khoa Ngoại (`data/patients_day.txt`)                | Diễn biến hoàn tất & gọi khám (`results/log_1phong_uutien.txt`)                                                                               | Hàng chờ Phòng 3 (`thường / ưu tiên`) | Đối chiếu tính tay   |
| --------- | --------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------- | -------------------- |
| `0 – 15`  | Không có                                                              | Phòng 3 trống (`dang kham=khong`)                                                                                                             | `0 / 0`                               | Khớp                 |
| `16`      | `BN54` (mức `2`, thời lượng `11`)                                     | `Goi kham: BN54 (phong 3, cho 0)`, dự kiến xong phút `16 + 11 = 27`                                                                           | `0 / 0`                               | Khớp                 |
| `21`      | `BN68` (mức `1`, thời lượng `10`)                                     | Phòng 3 đang khám `BN54` -> `BN68` vào `PriorityHeap` (không ngắt ca)                                                                         | `0 / 1`                               | Khớp                 |
| `27`      | Không có                                                              | `Hoan tat: BN54 (phong 3)` -> `Goi kham: BN68 (phong 3, cho 6)`, xong phút `27 + 10 = 37`                                                     | `0 / 0`                               | Khớp (`27 - 21 = 6`) |
| `37 – 46` | Không có                                                              | `Hoan tat: BN68 (phong 3)` tại phút `37`, phòng trống đến phút `46`                                                                           | `0 / 0`                               | Khớp                 |
| `47`      | `BN132` (mức `2`, thời lượng `11`)                                    | `Goi kham: BN132 (phong 3, cho 0)`, dự kiến xong phút `47 + 11 = 58`                                                                          | `0 / 0`                               | Khớp                 |
| `49`      | `BN158` (mức `1`, thời lượng `9`)                                     | Phòng 3 bận -> `BN158` vào `PriorityHeap`                                                                                                     | `0 / 1`                               | Khớp                 |
| `51`      | `BN74` (mức `0`, thời lượng `13`), `BN106` (mức `2`, thời lượng `13`) | Phòng 3 bận -> `BN74` vào `PriorityHeap`, `BN106` vào `LinkedQueue` (không ngắt ca `BN132`)                                                   | `1 / 2`                               | Khớp                 |
| `53`      | `BN10` (mức `1`, thời lượng `8`), `BN174` (mức `1`, thời lượng `13`)  | Phòng 3 bận -> cả hai vào `PriorityHeap`                                                                                                      | `1 / 4`                               | Khớp                 |
| `58`      | Không có                                                              | `Hoan tat: BN132 (phong 3)` -> `BN74` (mức `0`) vượt lên trước `BN158` (mức `1`): `Goi kham: BN74 (phong 3, cho 7)`, xong phút `58 + 13 = 71` | `1 / 3`                               | Khớp (`58 - 51 = 7`) |
| `59 – 60` | Không có                                                              | `BN74` tiếp tục khám đến phút `71`                                                                                                            | `1 / 3`                               | Khớp                 |

**Kết luận đối chiếu:** 100% các mốc hoàn tất, thứ tự ưu tiên gọi khám, thời gian chờ và độ dài hàng chờ từng phút khớp tuyệt đối giữa tính tay và nhật ký xuất ra từ chương trình.

---

### 3.3. Đối chiếu hàng chờ lớn nhất (`maxWaiting`) ở 1 phòng và 2 phòng

Nguồn đối chiếu: `results/log_1phong_uutien.txt`, tệp xuất thống kê Menu 8 (`saveRunResults`) và `results/comparison.txt` trên bộ `data/patients_day.txt` (bật ưu tiên, đóng nhận phút 480).

| Cấu hình phòng               | Đơn vị theo dõi           | `maxWaiting` từ nhật ký từng phút | `RoomStats.maxWaiting` (Menu 8) | `Maximum system waiting` (`comparison.txt`) | Kết quả |
| ---------------------------- | ------------------------- | --------------------------------- | ------------------------------- | ------------------------------------------- | ------- |
| **1 phòng/khoa** (`1\|1\|1`) | Phòng 1 (Nội)             | `10`                              | `10`                            | —                                           | Khớp    |
| **1 phòng/khoa** (`1\|1\|1`) | Phòng 2 (Nhi)             | `9`                               | `9`                             | —                                           | Khớp    |
| **1 phòng/khoa** (`1\|1\|1`) | Phòng 3 (Ngoại)           | `26`                              | `26`                            | —                                           | Khớp    |
| **1 phòng/khoa** (`1\|1\|1`) | **Toàn hệ thống**         | `41` (tại phút cao điểm)          | `41`                            | `41`                                        | Khớp    |
| **2 phòng/khoa** (`2\|2\|2`) | Phòng 1 / Phòng 2 (Nội)   | `2` / `1`                         | `2` / `1`                       | —                                           | Khớp    |
| **2 phòng/khoa** (`2\|2\|2`) | Phòng 3 / Phòng 4 (Nhi)   | `2` / `1`                         | `2` / `1`                       | —                                           | Khớp    |
| **2 phòng/khoa** (`2\|2\|2`) | Phòng 5 / Phòng 6 (Ngoại) | `2` / `2`                         | `2` / `2`                       | —                                           | Khớp    |
| **2 phòng/khoa** (`2\|2\|2`) | **Toàn hệ thống**         | `7`                               | `7`                             | `7`                                         | Khớp    |

---

### 3.4. Kiểm chứng quy tắc không ngắt ca (Non-preemption)

- Khi một phòng đã gọi bệnh nhân vào khám (`startTime = t`), phòng giữ trạng thái bận liên tục đến đúng phút `finishTime = t + duration` rồi mới giải phóng ở bước `finishIfDue(finishTime)` trước khi xét gọi ca kế tiếp ở bước `startNext(finishTime)`.
- Minh chứng thực tế trong `results/log_1phong_uutien.txt` (ảnh `docs/Giai Doan 4/img/a_nhat_ky.png`):
  - Tại phút `47`, Phòng 3 bắt đầu khám `BN132` (mức `2`, thời lượng `11`, `ket thuc=58`).
  - Tại phút `51`, bệnh nhân cấp cứu `BN74` (mức `0`) đến Phòng 3 nhưng không ngắt ca đang khám; `BN74` chờ trong `PriorityHeap` suốt `7` phút (`51` -> `58`) và chỉ được gọi ngay tại phút `58` sau dòng `Hoan tat: BN132 (phong 3)`.

---

## 4. Lỗi còn lại và việc chờ người khác

- **Không phát hiện lỗi logic** ở `LinkedQueue`, `ClinicRoom` hay luồng xử lý 1 phút của `Simulation`.
- **Việc chờ tích hợp:** Đang chờ PR #26 của thành viên C (`feat(c): add phase 4 logs and comparisons`) được Leader gộp vào nhánh `develop`. Sau khi kéo `develop` mới về nhánh `feature/g4-member-a`, cần bổ sung 1 dòng trả lời `n` (không ghi nhật ký) vào `tests/console/a_no_preempt.in` cho khớp menu 6 mới của C, chạy lại `mingw32-make test-all` và `sh tests/console/run_console_tests.sh` rồi mở PR vào `develop`.
