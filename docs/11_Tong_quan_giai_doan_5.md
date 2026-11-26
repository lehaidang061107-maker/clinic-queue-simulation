# TỔNG QUAN GIAI ĐOẠN 5

**Mục tiêu:** chứng minh chương trình đúng, không rò rỉ bộ nhớ, và đo được hiệu năng. Kết quả dùng cho chương Kiểm thử và Thực nghiệm của báo cáo.

## 1. Cần làm gì?

| # | Việc | Kết quả cần có |
|---|---|---|
| 1 | Chạy trên Windows và ASan | `make`, `test-all`, `test-console` đạt trên Windows; `make test-asan` đạt trên Linux/WSL |
| 2 | Bộ 10.000 bản ghi | `data/patients_large.txt`, sinh bằng menu 2: seed 2026, giờ đóng 480, thời lượng 5–15 |
| 3 | Đo thời gian chạy | `make bench` ra `results/bench_<máy>.csv`, đo trên ít nhất 2 máy |
| 4 | Đo bộ nhớ và rò rỉ | Bộ nhớ đỉnh theo N; ASan và `leaks` không lỗi trên bộ 10.000 |
| 5 | Tối ưu (nếu cần) | Chỉ làm khi số đo cho thấy đáng; kết quả mô phỏng không đổi |
| 6 | Bảng kiểm thử tổng hợp | Mỗi ca test một dòng: mục đích → mong đợi → thực tế → đạt |
| 7 | Biểu đồ và nhận xét | Thời gian theo N, bộ nhớ theo N, chờ TB theo mức × kịch bản; nhận xét đối chiếu độ phức tạp |

## 2. Quy tắc khi đo

- Kích thước N = 1.000, 2.000, 5.000, 10.000, 20.000; cả 4 cấu hình (1/2 phòng × bật/tắt ưu tiên).
- Mỗi điểm đo chạy 5 lần, lấy trung vị. Biên dịch `-O2`.
- Tắt in từng phút và nhật ký. Đo riêng: sinh dữ liệu, mô phỏng, Merge Sort, nạp/lưu tệp.
- Ghi kèm máy, hệ điều hành, compiler, cờ biên dịch, commit.
- Không đổi `DataGenerator` và kết quả mô phỏng: `results/comparison.txt` sinh lại phải trùng từng byte.

## 3. Thứ tự làm

| Mốc | Việc | Xong khi |
|---|---|---|
| 0 | Windows, ASan; A kiểm tra chéo ca `b_*` | Có kết quả từng máy |
| 1 | Bộ 10.000; viết `make bench` | `make bench` chạy hết, ra CSV |
| 2 | Đo thời gian, bộ nhớ, rò rỉ | Có số cho mọi N |
| 3 | Tối ưu (nếu cần) rồi đo lại | Có bảng trước/sau, `comparison.txt` không đổi |
| 4 | Biểu đồ, nhận xét, bảng kiểm thử; Leader duyệt | Đủ tiêu chí mục 5 |

## 4. Nộp gì?

- Mã: `tests/bench.cpp`, target `bench` trong Makefile, test bổ sung.
- Dữ liệu: `data/patients_large.txt`.
- Kết quả: `results/bench_<máy>.csv`, `results/memory_<máy>.txt`.
- Bàn giao: `docs/Giai Doan 5/G5_<người>_Ban_giao.md`; biểu đồ trong `docs/Giai Doan 5/img/`.

## 5. Tiêu chí duyệt

- `make`, `test-all`, `test-console` đạt trên macOS và Windows; `test-asan` đạt trên Linux/WSL.
- `patients_large.txt` đủ 10.000 bản ghi; sinh lại cùng seed ra đúng tệp; nạp → lưu → nạp lại không mất bản ghi.
- ASan và `leaks` không lỗi trên bộ 10.000.
- CSV đủ 5 kích thước × 4 cấu hình × 5 lần, trên ít nhất 2 máy.
- Có biểu đồ và nhận xét giải thích: vì sao 1 phòng/khoa tăng gần N², 2 phòng/khoa gần tuyến tính, và vì sao toàn chương trình không phải O(log n).
- Nếu có tối ưu: test đạt, `comparison.txt` trùng từng byte, có bảng trước/sau.
- Bảng kiểm thử phủ mọi ca trong `tests/` và `tests/console/`.

[Phân công giai đoạn 5](12_Phan_cong_giai_doan_5.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
