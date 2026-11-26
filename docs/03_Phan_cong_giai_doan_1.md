# PHÂN CÔNG GIAI ĐOẠN 1

Giai đoạn này chưa viết code. Mỗi người đọc header được giao, trả lời câu hỏi bằng lời của mình, làm ví dụ, rồi tạo một tệp `.md` trong `docs/Giai Doan 1/`. Chỗ chưa hiểu ghi cuối tệp để hỏi Leader.

## Leader — Cách cả chương trình hoạt động

**Header:** `Patient.h`, `SimulationConfig.h`, `Simulation.h`.

1. Bệnh nhân cần lưu gì? Thông tin nào có lúc tiếp nhận, thông tin nào chỉ có sau khi khám?
2. Một phút làm gì trước, gì sau? (hoàn tất ca → nhận người đến → xếp hàng → gọi khám → ghi trạng thái)
3. Nhiều phòng cùng khoa thì chọn phòng nào? Số người chờ bằng nhau thì sao?
4. Khi nào kết thúc? Hết giờ nhận mà còn người chờ thì làm gì?
5. Chạy lại có/không ưu tiên thì xóa gì, giữ gì?
6. Phần nào giữ dữ liệu bệnh nhân, phần nào chỉ giữ con trỏ? Ai giải phóng?

**Ví dụ:** 1 phòng; BN01 thường đến phút 0, BN02 thường đến phút 1, BN03 cấp cứu đến phút 2; mỗi người khám 5 phút. Lập bảng thứ tự khám, bắt đầu/kết thúc, thời gian chờ khi bật ưu tiên.

**Thêm:** sơ đồ các phần gọi nhau; kiểm tra thiết kế A/B/C khớp nhau.

**Gửi:** `G1_Leader_Thiet_ke.md`.

## A — Hàng đợi thường và phòng khám

**Header:** `LinkedQueue.h`, `ClinicRoom.h`.

1. Hàng đợi cần giữ gì để biết đầu hàng, cuối hàng, số người chờ?
2. Thêm người vào đâu, lấy người từ đâu?
3. Hàng rỗng thì sao? Lấy người cuối cùng cần cập nhật gì?
4. Phòng cần biết gì về người đang khám và giờ khám xong?
5. Phòng bận có gọi người mới không? Rảnh thì lấy heap hay hàng thường trước?
6. Danh sách liên kết so với mảng: thêm/lấy tốn bao nhiêu bước, bao nhiêu bộ nhớ?

**Ví dụ:** vẽ `BN01 → BN02 → BN03`, thêm BN04, gọi một người; vẽ lại sau mỗi bước. Thêm ví dụ lấy hết rồi thêm lại.

**Thêm:** các bước thêm/lấy/xóa hàng, bắt đầu/hoàn tất ca; ca rỗng, một người, nhiều người.

**Gửi:** `G1_A_Thiet_ke.md`.

## B — Ưu tiên và thống kê

**Header:** `PriorityHeap.h`, `Statistics.h`.

1. Ai khám trước? Cùng mức, cùng phút đến thì phân thứ tự bằng gì?
2. Heap lưu trên mảng thế nào? Vẽ cây và mảng; vì sao người cần lấy trước nằm ở gốc?
3. Thêm người hoặc lấy gốc thì đổi chỗ thế nào? Mảng đầy thì làm gì?
4. Heap so với danh sách luôn sắp xếp: thêm/lấy/tăng mảng tốn bao nhiêu?
5. Tính chờ trung bình/lớn nhất theo mức thế nào? Mức chưa có ai thì hiện gì?
6. Độ dài hàng có tính người đang khám không? Merge Sort xếp danh sách chờ giảm dần thế nào?

**Ví dụ:** BN01 (mức 1, phút 1, số 1), BN02 (mức 0, phút 2, số 2), BN03 (mức 1, phút 1, số 3): vẽ heap, ghi thứ tự lấy ra. Hai người cùng mức chờ 4 và 8: tính trung bình, lớn nhất.

**Thêm:** các bước push/pop/tăng mảng/sắp xếp; ca rỗng, hòa khóa, mảng đầy, chưa có số liệu.

**Gửi:** `G1_B_Thiet_ke.md`.

## C — Kho bệnh nhân, menu và tệp

**Header:** `PatientStore.h`, `FileIO.h`, `ConsoleUI.h`, `DataGenerator.h`.

1. Danh sách tất cả bệnh nhân lưu thế nào? Khác hàng chờ khám ở đâu?
2. Phát hiện mã hoặc số thứ tự trùng thế nào? Tốn bao nhiêu bước?
3. Menu có những mục nào? Nhập bệnh nhân hỏi những trường gì? (số thứ tự do chương trình cấp)
4. Nhập chữ vào tuổi, tuổi âm, mã trùng, khoa sai thì báo gì?
5. Một dòng tệp chứa gì, phân cách bằng gì? Thiếu tệp hoặc dòng sai thì làm gì?
6. Thoát thì lưu gì để mở lại còn người mới nhập? Tệp dữ liệu khác tệp kết quả ở đâu?
7. Sinh ≥30 và ≥10.000 bệnh nhân thế nào để mã không trùng, giá trị hợp lệ, cùng seed ra cùng dữ liệu?

**Ví dụ:** menu mẫu, 3 dòng bệnh nhân đúng định dạng, 3 trường hợp nhập sai kèm thông báo.

**Thêm:** các bước thêm/tìm bệnh nhân, đọc/ghi tệp; thời gian và bộ nhớ dự kiến.

**Gửi:** `G1_C_Thiet_ke.md`.

## Kiểm tra chéo

C đọc bài A, A đọc bài B, B đọc bài C; A đối chiếu ví dụ của Leader.

## Mẫu tệp thiết kế

1. **Cách làm:** trả lời câu hỏi.
2. **Các bước xử lý:** mã giả bằng lời, có thể kèm hình.
3. **Ví dụ và ca kiểm tra:** đầu vào → kết quả mong đợi.
4. **Thời gian và bộ nhớ:** theo từng thao tác.
5. **Nguồn tham khảo và câu hỏi còn vướng.**

[Tổng quan giai đoạn 1](02_Tong_quan_giai_doan_1.md)
