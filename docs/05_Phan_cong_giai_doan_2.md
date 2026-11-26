# PHÂN CÔNG GIAI ĐOẠN 2

Mỗi người viết code vào `.cpp` có sẵn trong `src/`, đúng tên hàm và kiểu trong header. Test đặt trong `tests/`, có `main` riêng. Ghi lệnh chạy và kết quả thực tế vào bàn giao.

## Leader — Dữ liệu chung và mô phỏng

**Viết:** `Patient.cpp`, `SimulationConfig.cpp`, `Simulation.cpp`, `main.cpp`, `Makefile`.

1. Phân loại mức ưu tiên; kiểm tra tuổi 0–120, khoa 1–3, ticket dương, phút đến trong giờ nhận, thời lượng dương, tràn số.
2. `initialize`: kiểm tra cấu hình và dữ liệu trước, rồi reset kho, phòng, thống kê, đồng hồ.
3. `step`: hoàn tất → xếp hết người đến trong phút → ghi hàng chờ → gọi khám → ghi hàng chờ.
4. `admit`: tự cấp ticket, phút đến = phút hiện tại; từ chối khi đã đóng nhận.
5. `isFinished`: không dừng khi còn người chờ hoặc đang khám sau giờ đóng.
6. `main` gọi `runConsole`; thống kê bắt đầu/hoàn tất mỗi người đúng một lần.

**Ca bắt buộc:** 1 phòng, BN01 thường đến 0, BN02 thường đến 1, BN03 cấp cứu đến 2, mỗi người 5 phút.

| Chế độ | Thứ tự khám | Bắt đầu BN01/02/03 | Chờ BN01/02/03 |
|---|---|---|---|
| Bật ưu tiên | BN01 → BN03 → BN02 | 0 / 10 / 5 | 0 / 9 / 3 |
| Tắt ưu tiên | BN01 → BN02 → BN03 | 0 / 5 / 10 | 0 / 4 / 8 |

Thêm: tuổi 5/6/59/60; khoa sai; hòa phòng; nhiều người cùng đến; cấp cứu không ngắt ca; nhận phút 479/480; chạy lại 4 cấu hình không cộng dồn.

**Gửi:** `tests/test_leader.cpp`, `G2_Leader_Ban_giao.md`.

## A — Hàng đợi thường và phòng khám

**Viết:** `LinkedQueue.cpp`, `ClinicRoom.cpp`.

1. FIFO: `enqueue`, `dequeue`, `peek`, `isEmpty`, `size`, `clear`. Lấy người cuối thì đưa đầu và cuối về `nullptr`; `clear` chỉ xóa nút.
2. Phòng: `waitingCount` = heap + FIFO, không tính người đang khám.
3. Bật ưu tiên: mức 0/1 vào heap, mức 2 vào FIFO; tắt: tất cả vào FIFO. Rảnh thì lấy heap trước.
4. `startNext` ghi giờ bắt đầu, giờ xong; `finishIfDue` trả người vừa xong đúng một lần.
5. `reset` dọn hàng và người đang khám, không xóa bệnh nhân.

**Ca bắt buộc:** FIFO BN01 → BN02 → BN03, thêm BN04 rồi lấy lần lượt; rỗng, một người, lấy hết rồi thêm lại. Phòng rỗng/bận, trước/đúng giờ xong, gọi `finishIfDue` hai lần, ví dụ 3 người của Leader.

**Gửi:** `tests/test_a.cpp`, `G2_A_Ban_giao.md`.

## B — Heap, thống kê và Merge Sort

**Viết:** `PriorityHeap.cpp`, `Statistics.cpp` (gồm `makeSortedWaitReport`).

1. Min-heap theo (mức, phút đến, ticket); `siftUp`, `siftDown`, `grow`.
2. Đầy thì tăng mảng; cấp phát thất bại không làm mất mảng cũ. `clear` không xóa bệnh nhân.
3. Thống kê: cộng thời gian chờ lúc bắt đầu khám; chờ trung bình/lớn nhất theo mức; hàng chờ lớn nhất theo phòng và toàn hệ thống.
4. Báo cáo chờ: Merge Sort tự cài, giảm dần theo chờ, hòa thì ticket tăng dần; không đổi thứ tự kho.

**Ca bắt buộc:** BN01 (1,1,1), BN02 (0,2,2), BN03 (1,1,3) phải lấy ra BN02 → BN01 → BN03. Rỗng, một phần tử, hòa khóa, vượt sức chứa, `clear` rồi dùng lại. Chờ 4 và 8 cùng mức → TB 6, max 8. Hai phòng có (3,0) và (0,4) → max hệ thống 4.

**Gửi:** `tests/test_b.cpp`, `G2_B_Ban_giao.md`.

## C — Kho, tệp, bộ sinh và menu

**Viết:** `PatientStore.cpp`, `FileIO.cpp`, `DataGenerator.cpp`, `ConsoleUI.cpp`.

1. Kho: danh sách liên kết đơn, chèn theo (phút đến, ticket), chặn trùng mã và ticket.
2. Nạp tệp: kiểm tra tiêu đề và đủ 8 trường phân cách `|`; dòng sai báo số dòng và lý do, vẫn nhận các dòng đúng.
3. Lưu đầu vào và lưu kết quả riêng; ghi lỗi thì báo rõ, không báo "đã lưu".
4. Bộ sinh: cùng seed ra cùng dữ liệu trên mọi hệ điều hành; mã, ticket duy nhất; thời lượng 5–15.
5. Menu: nạp/sinh, chọn cấu hình, tiếp nhận, chạy 1 phút/chạy hết, xem, lưu, thoát. Đọc số kiểm tra cả chuỗi, xử lý nhập chữ, tràn số, EOF. Menu chỉ gọi `Simulation`, không tự xếp hàng.

**Ca bắt buộc:** 3 dòng đúng; tuổi chữ/âm; mã, ticket trùng; khoa sai; phút đến 480; thiếu trường; tiêu đề sai; thiếu tệp; dòng sai xen dòng đúng; đường dẫn không ghi được; sinh lại cùng seed.

**Gửi:** `tests/test_c.cpp`, `G2_C_Ban_giao.md`.

## Kiểm tra chéo

C chạy test của A, A chạy test của B, B chạy test của C.

## Mẫu bàn giao

1. **Đã cài gì:** hàm, tệp.
2. **Cách chạy:** máy, hệ điều hành, compiler, lệnh.
3. **Kết quả:** bảng ca → mong đợi → thực tế → đạt.
4. **Còn lại:** lỗi chưa sửa, việc chờ người khác.

[Tổng quan giai đoạn 2](04_Tong_quan_giai_doan_2.md)
