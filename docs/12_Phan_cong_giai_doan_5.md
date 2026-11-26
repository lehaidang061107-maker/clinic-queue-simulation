# PHÂN CÔNG GIAI ĐOẠN 5

Mỗi người làm trên nhánh riêng, mở PR vào **`develop`**. Trước khi mở PR: chạy `make test-all` và `make test-console`, dán dòng tổng kết vào bàn giao.

## Leader

1. Duyệt `make bench` của B: đúng `-O2`, tắt in từng phút và nhật ký, tách thời gian sinh dữ liệu.
2. ~~Quyết định tối ưu~~ — **đã làm**, xem [bàn giao Leader G5](Giai%20Doan%205/G5_Leader_Ban_giao.md). B cần đo lại bằng `make bench` để có bảng trước/sau chính thức (bản trước: commit `08345d0`). Chi tiết cách làm:
   - nếu làm: giữ con trỏ chạy dọc kho và bộ đếm người chưa xong thay vì duyệt lại cả kho mỗi phút; kiểm `test-all`, `test-console`, `comparison.txt` trùng từng byte; báo B đo lại;
   - nếu không làm: ghi lý do kèm số đo.
3. Viết nháp mục **Độ phức tạp và hiệu năng** cho báo cáo, đối chiếu với số đo của B.
4. Cập nhật bảng môi trường trong README theo kết quả của C.
5. Duyệt PR theo thứ tự: C → B → A → Leader.

## A — Bảng kiểm thử và bộ nhớ

1. Chạy hai ca `b_empty_avg`, `b_format_example_2` của B; ghi kết quả vào `G4_B_Ban_giao.md`.
2. Thêm test quy mô lớn vào `test_a.cpp`:
   - `LinkedQueue` nhận rồi lấy 100.000 phần tử, đúng thứ tự FIFO;
   - `ClinicRoom` nhận 10.000 người đủ 3 mức, gọi hết, đúng thứ tự ở cả hai chế độ.
3. Đo bộ nhớ:
   - lý thuyết: `sizeof` các nút và `Patient`, tính byte dự kiến cho từng N;
   - thực tế: bộ nhớ đỉnh từng N bằng `/usr/bin/time -l` (macOS) hoặc `/usr/bin/time -v` (Linux), chạy `./build/bench --sizes <N> --reps 1 --out build/mem.csv` (sau `make bench-quick` để có `build/bench`);
   - ghi vào `results/memory_<máy>.txt`.
4. Lập **bảng kiểm thử tổng hợp**: mỗi ca trong `tests/` và `tests/console/` một dòng (mã ca → mục đích → dữ liệu → mong đợi → thực tế → đạt).
5. Chụp ảnh `make bench` và `make test-asan` cho slide.

**Gửi:** `G5_A_Ban_giao.md`, `results/memory_<máy>.txt`, bảng kiểm thử.

## B — Đo hiệu năng

1. Viết `tests/bench.cpp` và target `make bench` (biên dịch `-O2`):
   - N = 1.000, 2.000, 5.000, 10.000, 20.000, sinh bằng `generatePatients` seed 2026;
   - 5 lần × 4 cấu hình; mỗi lần: sinh → lưu → nạp lại một lần, rồi 4 cấu hình chạy trên cùng kho (chỉ `initialize` lại, không sinh lại);
   - đo bằng `std::chrono::steady_clock`: sinh dữ liệu, mô phỏng, Merge Sort, nạp và lưu tệp;
   - chạy `make bench MACHINE=<máy>` (Windows: `mingw32-make bench MACHINE=windows`), ghi `results/bench_<máy>.csv`; `make bench-quick` để thử nhanh. Cột `n,rooms,priority,run,gen_ms,sim_ms,sort_ms,load_ms,save_ms,end_minute`; đầu tệp ghi máy, hệ điều hành, compiler, cờ, commit.
2. Đo trên ít nhất 2 máy (macOS của Leader, Windows của C).
3. Vẽ biểu đồ vào `docs/Giai Doan 5/img/`:
   - thời gian mô phỏng theo N, 4 cấu hình, trục log–log;
   - thời gian Merge Sort theo N, so với N log N;
   - cột chờ TB theo mức × kịch bản từ `results/comparison.txt`.
4. Viết nhận xét, mỗi ý dẫn số từ CSV: độ dốc 1 phòng và 2 phòng/khoa, Merge Sort có khớp N log N không, bảng trước/sau nếu Leader tối ưu.

**Gửi:** `G5_B_Ban_giao.md`, `results/bench_<máy>.csv`, biểu đồ, nhận xét.

## C — Windows, ASan và bộ 10.000 bản ghi

1. **Windows:** cài `make` cho MSYS2 nếu chưa có (`pacman -S mingw-w64-ucrt-x86_64-make`), chạy `mingw32-make`, `mingw32-make test-all`, `mingw32-make test-console`.
2. **ASan:** cài WSL Ubuntu (`wsl --install -d Ubuntu`) và `build-essential`, clone repo vào `~/Lab`, chạy `make test-asan`. Không cài được WSL thì báo Leader ngay.
3. **`data/patients_large.txt`:** sinh bằng menu 2 (10.000 người, seed 2026, giờ đóng 480, thời lượng 5–15). Ghi bảng phân bố theo khoa và mức vào bàn giao.
4. Thêm vào `test_c.cpp`:
   - sinh lại cùng cấu hình ra đúng tệp;
   - nạp → lưu → nạp lại đủ 10.000 bản ghi, từng trường khớp.
5. Chạy bản ASan trong WSL với `patients_large.txt` qua menu 9, ghi kết quả.
6. Chạy `make bench` trên Windows, gửi `results/bench_windows.csv` cho B.

Với mỗi máy, ghi hệ điều hành, compiler, commit và dòng tổng kết.

**Gửi:** `G5_C_Ban_giao.md`, `data/patients_large.txt`, kết quả Windows và ASan.

## Kiểm tra chéo

- A chạy lại `make bench` của B trên máy mình.
- B chạy test quy mô lớn của A.
- C chạy lại toàn bộ test trên Windows sau khi mọi PR giai đoạn 5 đã merge.

## Mẫu bàn giao

1. **Đã làm gì:** tệp sửa, ứng với việc nào.
2. **Cách chạy:** máy, hệ điều hành, compiler, cờ, lệnh, dữ liệu, commit.
3. **Kết quả:** bảng ca → mong đợi → thực tế → đạt.
4. **Còn lại:** lỗi chưa sửa, việc chờ người khác.

[Tổng quan giai đoạn 5](11_Tong_quan_giai_doan_5.md)
