# VIỆC TỒN CHUYỂN SANG GIAI ĐOẠN 5

## 1. Chạy kiểm tra trên Windows và ASan

**Người nhận: C (haidangbuitran67-collab).** Lý do: phần code của C ổn định nhất qua giai đoạn 2–4. `PatientStore`/`FileIO` kiểm tra kỹ mã trùng, ticket trùng, tràn số, lỗi cấp phát, dòng hỏng, CRLF và đường dẫn không ghi được; `test_c` phủ cả các trường hợp lỗi và tái sinh `patients_day.txt` đúng từng byte; phần lớn nhất của G4 (nhật ký ra tệp, bảng menu 9) được merge mà gần như không phải sửa. C cũng đã có sẵn môi trường Windows (MSYS2). Chi tiết cách làm ở [phân công giai đoạn 5, mục C](12_Phan_cong_giai_doan_5.md).

Đến nay mọi kết quả trong repo do Leader chạy đều trên macOS. Bàn giao của A, B, C có ghi chạy trên Windows nhưng chưa ai chạy lại trên `develop` sau khi gộp đủ B + C + A. Chưa ai chạy ASan.

Việc cần làm, trên `develop` sau khi 3 PR giai đoạn 4 đã merge:

1. Windows: `mingw32-make`, `mingw32-make test-all`, `mingw32-make test-console` (cần `sh` của Git Bash/MSYS2). Ghi máy, compiler, commit đã chạy, và dòng tổng kết.
2. ASan: `make test-asan` trên WSL Ubuntu (MinGW không hỗ trợ ASan). Máy macOS của Leader dừng ngay lúc ASan khởi động (lỗi toolchain, chưa chạy tới code nhóm), nên đã thay bằng `leaks` và UBSan. Không cài được WSL thì báo Leader để chạy trên GitHub Actions.
3. Ghi kết quả vào `docs/Giai Doan 4/G4_C_Ban_giao.md` (mục kiểm tra) và cập nhật bảng môi trường trong `README.md`. Máy nào chưa chạy được thì ghi rõ lý do.

## 2. Khôi phục phần giai đoạn 4 của B

**Người nhận: Leader.** Commit `5c0fb23` (định sửa 1 dòng trong `a_no_preempt.in`) đã ghi đè lại nhiều tệp, làm `develop` mất `G4_B_Ban_giao.md`, các hàm chờ TB chung và test G4 của B, hai ca `b_empty_avg`/`b_format_example_2`, và các mục hướng dẫn trong README. **Đã khôi phục (26/11)** từ commit `1d7f762`, giữ dòng sửa của `a_no_preempt.in`. Sau khôi phục: `make test-all` đạt (có lại nhóm chờ TB chung của B), `make test-console` 25/25, `comparison.txt` sinh lại trùng từng byte, `leaks` 0 rò rỉ. Còn lại: A kiểm tra chéo hai ca `b_*`, xem [phân công](12_Phan_cong_giai_doan_5.md).

## 3. Ghi chú về số liệu đỉnh hàng chờ toàn hệ thống

Số trong `results/comparison.txt` (41 với 1 phòng/khoa, 7 với 2 phòng/khoa) là số đúng. Nếu cộng thô từ nhật ký sẽ ra thấp hơn (ví dụ 6 thay vì 7), vì `Simulation::step()` đo hàng chờ hai lần mỗi phút: sau khi tiếp nhận (trước khi gọi khám) và sau khi gọi khám. Nhật ký chỉ in snapshot sau khi gọi khám, nên bỏ sót người vừa được gọi trong chính phút đó. Khi đối chiếu `maxWaiting` với nhật ký ở giai đoạn sau, cần tính đến điểm đo này.
