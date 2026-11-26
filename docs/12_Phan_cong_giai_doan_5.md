# PHÂN CÔNG GIAI ĐOẠN 5

**Giai đoạn này biến chương trình đã đúng thành bằng chứng: kiểm thử, bộ nhớ và hiệu năng.** Leader là bạn; A/B/C giữ phần phụ trách như trước. B chủ trì thực nghiệm. Mỗi người làm mốc 0 (nợ giai đoạn 4) trước, rồi mới sang việc giai đoạn 5.

## 1. Bắt đầu và gửi bài thế nào?

1. Đọc [tổng quan giai đoạn 5](11_Tong_quan_giai_doan_5.md) và bàn giao G4 của phần mình.
2. Kéo `develop` mới (đã khôi phục phần G4 của B). Làm trên nhánh riêng theo [quy trình Git](../MEMBER_GIT_WORKFLOW.md), mở PR vào **`develop`**.
3. Trước khi mở PR: chạy `make test-all` và `make test-console`, dán dòng tổng kết vào bàn giao. Xem `git diff --stat develop` để chắc PR chỉ chứa tệp mình định sửa; nếu thấy tệp lạ bị xoá hoặc đổi thì dừng lại và hỏi Leader (lỗi mất phần B ở G4 xảy ra đúng theo cách này).
4. Gửi Leader mã, test, kết quả và tệp bàn giao; ghi rõ phần chưa xong.

```text
docs/Giai Doan 5/
├── G5_Leader_Ban_giao.md
├── G5_A_Ban_giao.md
├── G5_B_Ban_giao.md
├── G5_C_Ban_giao.md
└── img/                     # Biểu đồ thời gian, bộ nhớ, chờ theo mức

data/patients_large.txt      # 10.000 bản ghi (C)
tests/bench.cpp              # Chương trình đo (B)
results/
├── bench_<máy>.csv          # Thời gian chạy (B, mỗi máy một tệp)
└── memory_<máy>.txt         # Bộ nhớ đỉnh theo N (A)
```

**Giao diện dùng chung cần chốt sớm (mốc 1):** định dạng cột của `bench_<máy>.csv` (B đề xuất, Leader duyệt) để A và C ghi số liệu cùng một kiểu. Đề xuất: `n,rooms,priority,run,gen_ms,sim_ms,sort_ms,load_ms,save_ms,end_minute`.

## 2. Leader — Khôi phục, tối ưu có kiểm chứng và tích hợp

**Sửa:** `src/Statistics.cpp`, `include/Statistics.h`, `src/ConsoleUI.cpp`, `tests/test_b.cpp`, `tests/console/b_*` (chỉ ở mốc 0, để khôi phục); `src/Simulation.cpp`, `include/Simulation.h` (nếu tối ưu); `tests/test_leader.cpp`, `Makefile`, `README.md`.

**Gửi:** `G5_Leader_Ban_giao.md`, nháp mục **Độ phức tạp và hiệu năng** cho báo cáo.

Mốc 0:

1. ~~Khôi phục phần G4 của B~~ bị commit `5c0fb23` ghi đè — **đã xong 26/11** từ commit `1d7f762`, giữ dòng sửa của `a_no_preempt.in`. Kết quả: `test-all` đạt, `test-console` 25/25, `comparison.txt` trùng từng byte. Báo cả nhóm kéo `develop` mới.
2. Gom kết quả Windows và ASan của C, cập nhật bảng môi trường trong README.

Giai đoạn 5:

1. **Duyệt `make bench`** của B: đúng cờ `-O2`, tắt in từng phút và nhật ký, tách thời gian sinh dữ liệu khỏi thời gian mô phỏng.
2. **Quyết định tối ưu** sau khi có số đo đầu tiên của B. Nếu phần tìm người đến và `isFinished()` (O(T·N)) chiếm ưu thế:
   - giữ một con trỏ chạy dọc kho (kho đã sắp theo phút đến) và một bộ đếm người chưa xong, thay cho duyệt lại cả kho mỗi phút;
   - cả ngày giảm từ O(T·(N + R) + N·(R + log q)) xuống O(T·R + N·(R + log q));
   - kiểm chứng: `test-all`, `test-console` đạt; `comparison.txt` trùng từng byte; thêm test so lịch khám từng người trước/sau trên bộ 10.000;
   - báo B đo lại để có bảng trước/sau.

   Nếu không đáng (ví dụ phần mô phỏng dưới 1 s ở N = 20.000) thì không sửa, ghi lý do kèm số đo.
3. **Nháp mục Độ phức tạp và hiệu năng:** bảng độ phức tạp thời gian và bộ nhớ từng thao tác chính (lấy từ [nháp Mô phỏng G4](Giai%20Doan%204/G4_Leader_Mo_phong_nhap.md#3-độ-phức-tạp)), đối chiếu với số đo của B. Giải thích vì sao toàn chương trình không phải O(log n).
4. Duyệt PR theo thứ tự: C (dữ liệu) → B (bench) → A → Leader (tối ưu, nếu có).

## 3. A — Bảng kiểm thử tổng hợp và đo bộ nhớ

**Sửa:** `tests/test_a.cpp`; `src/LinkedQueue.cpp`, `src/ClinicRoom.cpp` (chỉ khi phát hiện lỗi).

**Gửi:** `G5_A_Ban_giao.md`, `results/memory_<máy>.txt`, bảng kiểm thử tổng hợp.

Mốc 0:

1. Kiểm tra chéo hai ca mới của B (`b_empty_avg`, `b_format_example_2`) đã được khôi phục; ghi kết quả vào mục kiểm tra chéo trong `G4_B_Ban_giao.md`.

Giai đoạn 5:

1. **Kiểm thử quy mô lớn cho phần mình** trong `test_a.cpp`:
   - một `LinkedQueue` nhận rồi lấy ra 100.000 phần tử, thứ tự FIFO đúng, `clear` giải phóng hết;
   - một `ClinicRoom` nhận 10.000 người (đủ 3 mức), gọi hết, thứ tự đúng theo chế độ bật/tắt ưu tiên.
2. **Đo bộ nhớ:**
   - Lý thuyết: in `sizeof` nút kho, nút FIFO, `Patient`, con trỏ heap; tính số byte dự kiến cho N = 1.000 … 20.000.
   - Thực tế: bộ nhớ đỉnh cho từng N bằng `/usr/bin/time -l` (macOS) hoặc `/usr/bin/time -v` (Linux/WSL), chạy chương trình đo của B ở 1 phòng/khoa bật ưu tiên.
   - Ghi vào `results/memory_<máy>.txt` và so sánh lý thuyết với thực tế.
3. **Bảng kiểm thử tổng hợp cho báo cáo:** mỗi ca unit test và ca menu trong `tests/` một dòng: mã ca → mục đích → dữ liệu → mong đợi → thực tế → đạt. Gom từ bàn giao G2–G5 của cả nhóm; ca nào thiếu thông tin thì hỏi người viết ca đó.
4. Giữ sơ đồ và ảnh cho slide; chụp thêm ảnh `make bench` và `make test-asan` (nhận ảnh ASan từ C).

## 4. B — Chủ trì thực nghiệm hiệu năng

**Sửa:** `tests/bench.cpp` (mới), `Makefile` (thêm target `bench`, báo Leader), `src/Statistics.cpp` (chỉ khi phát hiện lỗi).

**Gửi:** `G5_B_Ban_giao.md`, `results/bench_<máy>.csv`, biểu đồ trong `docs/Giai Doan 5/img/`, nhận xét.

Mốc 0:

1. Kiểm tra lại phần G4 của mình đã được khôi phục trên `develop`: `test_b` đủ 4 nhóm, hai ca `b_*` đạt, bàn giao G4 còn nguyên.

Giai đoạn 5:

1. **Viết `tests/bench.cpp` và `make bench`** (biên dịch `-O2`, liên kết mô-đun thật):
   - N = 1.000, 2.000, 5.000, 10.000, 20.000, sinh bằng `generatePatients` seed 2026, giờ đóng 480, thời lượng 5–15;
   - mỗi N chạy 4 cấu hình, mỗi cấu hình 5 lần; giữa các lần chỉ `initialize` lại, không sinh lại dữ liệu;
   - đo bằng `std::chrono::steady_clock` từng phần: sinh dữ liệu, mô phỏng, Merge Sort danh sách chờ, nạp và lưu tệp (dùng `FileIO` của C, ghi vào `build/`);
   - không in từng phút, không ghi nhật ký;
   - in bảng trung vị ra màn hình và ghi toàn bộ lần đo vào `results/bench_<máy>.csv`; đầu tệp ghi máy, hệ điều hành, compiler, cờ biên dịch, commit, ngày.
2. **Đo trên ít nhất 2 máy** (ví dụ macOS của Leader và Windows của C). Không chạy việc nặng khác khi đo.
3. **Biểu đồ:**
   - thời gian mô phỏng theo N, 4 đường cho 4 cấu hình, trục log–log để thấy độ dốc (≈ 1 là tuyến tính, ≈ 2 là bình phương);
   - thời gian Merge Sort theo N, so với đường N log N;
   - cột chờ TB theo mức × kịch bản từ `results/comparison.txt` (biểu đồ còn thiếu từ G4).
4. **Nhận xét** (dùng cho báo cáo), mỗi ý dẫn số từ CSV:
   - 1 phòng/khoa: thời gian tăng gần N² vì phút kết thúc T tăng theo N (dẫn cột `end_minute`);
   - 2 phòng/khoa: so sánh độ dốc với 1 phòng/khoa;
   - Merge Sort có khớp N log N không;
   - nếu Leader tối ưu: bảng trước/sau và độ dốc mới.

## 5. C — Windows, ASan và bộ 10.000 bản ghi

**C nhận hai việc kiểm tra quan trọng nhất** (Windows và ASan) vì phần code của C ở giai đoạn 2–4 ổn định nhất: kho và đọc/ghi tệp kiểm tra kỹ mọi đầu vào sai, test phủ cả trường hợp lỗi, và phần menu 9/nhật ký lớn nhất G4 được merge mà gần như không phải sửa. C cũng đã có sẵn môi trường Windows (MSYS2).

**Sửa:** `data/patients_large.txt`, `tests/test_c.cpp`; `src/FileIO.cpp`, `src/PatientStore.cpp` (chỉ khi phát hiện lỗi); `README.md` (mục Biên dịch, phối hợp với Leader).

**Gửi:** `G5_C_Ban_giao.md`, dữ liệu, kết quả Windows và ASan.

Mốc 0:

1. **Windows:**
   - Cài `make` cho MSYS2 nếu chưa có (`pacman -S mingw-w64-ucrt-x86_64-make`), để chạy đúng lệnh Makefile thay vì biên dịch tay như G4.
   - Chạy `mingw32-make`, `mingw32-make test-all`, `mingw32-make test-console`.
   - Ghi máy, phiên bản Windows, compiler, commit, dòng tổng kết.
2. **ASan** (MinGW không hỗ trợ ASan, nên dùng WSL):
   - Cài WSL Ubuntu (`wsl --install -d Ubuntu`), trong đó cài `build-essential`.
   - Clone repo vào thư mục của WSL (ví dụ `~/Lab`, không dùng `/mnt/c/...` để tránh lỗi quyền và chậm), checkout cùng commit với lần chạy Windows.
   - Chạy `make test-asan`. Ghi phiên bản Ubuntu, gcc, commit và dòng cuối.
   - Nếu không cài được WSL: báo Leader ngay để chuyển sang chạy trên GitHub Actions (ubuntu-latest), không để tồn sang giai đoạn 6.
3. Ghi kết quả vào mục Kiểm tra của `G4_C_Ban_giao.md` và báo Leader cập nhật bảng môi trường README.

Giai đoạn 5:

1. **`data/patients_large.txt`:** sinh bằng menu 2 với 10.000 người, seed 2026, giờ đóng 480, thời lượng 5–15. Không chỉnh tay. Ghi bảng phân bố (theo khoa, theo mức, tổng thời lượng mỗi khoa) vào bàn giao.
2. **Test trong `test_c.cpp`:**
   - sinh lại cùng cấu hình ra đúng tệp (giống kiểm tra `patients_day.txt`);
   - nạp → lưu → nạp lại: đủ 10.000 bản ghi, từng trường trùng khớp.
3. **Không rò rỉ trên bộ lớn:** trong WSL, build bản ASan (`build/clinic_simulator_asan` từ `make test-asan`), nạp `patients_large.txt` và chạy menu 9; trên macOS nhờ Leader chạy `leaks --atExit` với cùng thao tác. Ghi kết quả.
4. **Đo trên Windows cho B:** chạy `make bench` trên máy Windows, gửi `results/bench_windows.csv`.

## 6. Kiểm tra chéo và bàn giao

Lệnh chung từ thư mục gốc:

```bash
make
make test-all
make test-console
make bench
```

Windows dùng `mingw32-make`; `test-console` cần `sh` của Git Bash hoặc MSYS2. `test-asan` chạy trên Linux/WSL.

Mỗi tệp bàn giao G5 gồm:

1. **Đã làm gì:** tệp sửa, ứng với mục nào ở tổng quan.
2. **Cách chạy:** máy, hệ điều hành, compiler, cờ biên dịch, lệnh, dữ liệu, commit.
3. **Kết quả:** bảng ca → mong đợi → thực tế → đạt/chưa. Số liệu hiệu năng ghi rõ lấy từ tệp nào trong `results/`.
4. **Lỗi còn lại và việc chờ người khác.**

Kiểm tra chéo giai đoạn 5: **A chạy lại `make bench` của B trên máy mình; B chạy test quy mô lớn của A; C chạy lại toàn bộ test trên Windows sau khi mọi PR G5 đã merge.** Người kiểm tra ghi kết quả vào mục kiểm tra chéo trong bàn giao của người kia.

Leader chạy lại toàn bộ test và ca menu, rà CSV với nhận xét của B, rồi chỉ duyệt khi đạt [tiêu chí hoàn thành](11_Tong_quan_giai_doan_5.md#6-khi-nào-được-duyệt-xong-giai-đoạn-5).

**Chuẩn bị cho giai đoạn 6:** A giữ sơ đồ, ảnh và bảng kiểm thử cho slide; B giữ biểu đồ và nhận xét cho chương Thực nghiệm; C giữ README và bắt đầu gom nội dung báo cáo; Leader giữ nháp Mô phỏng và Độ phức tạp.

[Tổng quan giai đoạn 5](11_Tong_quan_giai_doan_5.md) · [Phân công giai đoạn 4](09_Phan_cong_giai_doan_4.md)
