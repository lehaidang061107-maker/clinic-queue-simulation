# PHÂN CÔNG GIAI ĐOẠN 1

**Giai đoạn này chưa cần viết code hoàn chỉnh.** Mỗi người đọc phần được giao, giải thích cách làm bằng lời của mình và làm ví dụ nhỏ. Leader là bạn; A/B/C là ba thành viên còn lại.

## 1. Bắt đầu và gửi bài thế nào?

1. Đọc [quy tắc chung](../README.md#quy-tắc-chính) và câu hỏi dành cho mình bên dưới.
2. Mở các tệp `.h` được chỉ định trong `include/`. Tệp `.h` liệt kê dữ liệu và tên hàm mà sau này mình sẽ viết code.
3. Tạo một tệp `.md` trong **`docs/Giai Doan 1/`**, trả lời câu hỏi và làm ví dụ. Chỗ chưa hiểu ghi vào cuối tệp để hỏi Leader.
4. Sau khi hiểu cách làm, bổ sung các bước xử lý, thời gian/bộ nhớ và trường hợp cần kiểm tra. Leader hỗ trợ rà lại.

```text
docs/Giai Doan 1/
├── G1_Leader_Thiet_ke.md
├── G1_A_Thiet_ke.md
├── G1_B_Thiet_ke.md
├── G1_C_Thiet_ke.md
└── images/                 # Ảnh nếu có, ví dụ A_fifo.png
```

Mỗi người chỉ sửa tệp của mình. Có thể vẽ bằng chữ ngay trong `.md`; không bắt buộc vẽ ảnh. Code vẫn ở `include/` và `src/`. Đây là bàn giao cho Leader, không phải báo cáo Word hoặc bài nộp riêng cho thầy.

## 2. Leader — Cách cả chương trình hoạt động

**Mở:** `Patient.h`, `SimulationConfig.h`, `Simulation.h`.
**Gửi:** `G1_Leader_Thiet_ke.md`.

Trả lời:

1. Một bệnh nhân cần lưu thông tin gì? Thông tin nào có lúc tiếp nhận, thông tin nào chỉ có sau khi khám?
2. Một phút chương trình làm gì trước, làm gì sau? Dùng thứ tự: hoàn tất ca → nhận người đến → xếp hàng → gọi khám → ghi trạng thái.
3. Có nhiều phòng cùng khoa thì chọn phòng nào? Nếu số người chờ bằng nhau thì sao?
4. Chương trình kết thúc lúc nào? Hết giờ nhận nhưng còn người chờ thì xử lý thế nào?
5. Chạy lại có/không ưu tiên thì cần xóa kết quả cũ nào và giữ nguyên dữ liệu nào?
6. Phần nào giữ dữ liệu bệnh nhân, phần nào chỉ dùng con trỏ tham chiếu? Ai giải phóng dữ liệu đó?

**Ví dụ cần làm:** một phòng, ba người: BN01 thường đến phút 0, BN02 thường đến phút 1, BN03 cấp cứu đến phút 2; mỗi người khám 5 phút. Lập bảng thứ tự khám, phút bắt đầu/kết thúc và thời gian chờ khi có ưu tiên. BN01 đang khám không bị ngắt.

**Leader kiểm tra cuối:** thiết kế của A/B/C khớp nhau, không tự đổi quy tắc đã chốt; ghi sơ đồ các phần gọi nhau và duyệt header trước khi code.

## 3. A — Hàng đợi thường và phòng khám

**Mở:** `LinkedQueue.h`, `ClinicRoom.h`; xem `Patient.h` để hiểu dữ liệu bệnh nhân.
**Gửi:** `G1_A_Thiet_ke.md`.

Trả lời:

1. Hàng đợi cần giữ những gì để biết đầu hàng, cuối hàng và số người chờ?
2. Người mới thêm vào đâu? Người được gọi lấy từ đâu?
3. Hàng rỗng thì xử lý thế nào? Lấy người cuối cùng ra cần cập nhật gì?
4. Một phòng cần biết thông tin gì về người đang khám và thời điểm khám xong?
5. Phòng đang bận có gọi người mới được không? Khi rảnh thì lấy heap ưu tiên hay hàng thường trước?
6. Dùng DSLK thuận lợi/bất lợi gì so với lưu hàng đợi trên mảng? Thêm/lấy cần bao nhiêu bước và bao nhiêu bộ nhớ?

**Ví dụ cần làm:** vẽ `BN01 → BN02 → BN03`, thêm BN04 rồi gọi một người. Vẽ lại hàng sau mỗi bước; thêm ví dụ lấy hết rồi thêm lại.

**Bổ sung:** viết các bước thêm/lấy/xóa hàng, bắt đầu/hoàn tất ca; liệt kê ca rỗng, một người, nhiều người. A chỉ quản lý trong một phòng; chọn bệnh nhân vào phòng nào do Leader làm.

## 4. B — Ưu tiên và thống kê

**Mở:** `PriorityHeap.h`, `Statistics.h`; xem `Patient.h` để hiểu mức ưu tiên.
**Gửi:** `G1_B_Thiet_ke.md`.

Trả lời:

1. Ai được khám trước? Nếu cùng mức ưu tiên, cùng phút đến thì dùng gì để phân thứ tự?
2. Heap được lưu trên mảng thế nào? Vẽ cây nhỏ và mảng tương ứng; vì sao người cần lấy trước nằm ở gốc?
3. Khi thêm người hoặc lấy người ở gốc, cần đổi vị trí thế nào để giữ đúng heap? Mảng đầy thì làm gì?
4. Heap có lợi/bất lợi gì so với danh sách luôn được xếp theo ưu tiên? Thêm/lấy và tăng mảng tốn bao nhiêu bước, bộ nhớ?
5. Tính thời gian chờ trung bình/lớn nhất theo từng mức thế nào? Nếu một mức chưa có bệnh nhân thì hiển thị gì?
6. Độ dài hàng đợi có tính người đang khám không? Dùng Merge Sort để xếp danh sách kết quả theo thời gian chờ giảm dần thế nào?

**Ví dụ cần làm:** BN01 mức 1 đến phút 1, số 1; BN02 mức 0 đến phút 2, số 2; BN03 mức 1 đến phút 1, số 3. Vẽ heap và ghi thứ tự lấy ra. Với hai người cùng mức có thời gian chờ 4 và 8 phút, tính trung bình và lớn nhất.

**Bổ sung:** viết các bước push/pop/tăng mảng và sắp xếp; liệt kê ca rỗng, hòa khóa, mảng đầy, chưa có số liệu. Giải thích trường hợp tăng mảng có thể tốn nhiều thời gian hơn một lần thêm thông thường.

## 5. C — Lưu bệnh nhân, menu và tệp

**Mở:** `PatientStore.h`, `FileIO.h`, `ConsoleUI.h`, `DataGenerator.h`; xem `Patient.h` và `SimulationConfig.h`.
**Gửi:** `G1_C_Thiet_ke.md`.

Trả lời:

1. Danh sách tất cả bệnh nhân được lưu thế nào? Phân biệt danh sách này với hàng người đang chờ khám.
2. Làm sao phát hiện mã hoặc số thứ tự bị trùng? Tìm trong danh sách cần duyệt bao nhiêu người?
3. Menu có những lựa chọn nào? Khi nhập bệnh nhân cần hỏi các trường gì? Số thứ tự do chương trình tự cấp.
4. Nếu nhập chữ vào ô tuổi, tuổi âm, mã trùng hoặc chuyên khoa sai thì báo gì?
5. Một dòng tệp bệnh nhân chứa gì, phân tách bằng dấu nào? Khi thiếu tệp hoặc gặp dòng sai thì xử lý thế nào?
6. Khi thoát cần lưu gì để mở lại vẫn có người mới nhập? Tệp dữ liệu khác tệp kết quả ở đâu?
7. Sau này sẽ sinh >=30 và >=10.000 bệnh nhân thế nào: mã không trùng, phút đến/thời lượng hợp lệ, cùng seed tạo cùng dữ liệu?

**Ví dụ cần làm:** viết menu mẫu, 3 dòng bệnh nhân đúng định dạng và 3 trường hợp nhập sai kèm thông báo mong đợi. Có thể dùng lại `data/format_example.txt`; giai đoạn này chưa cần tạo bộ dữ liệu lớn.

**Bổ sung:** viết các bước thêm/tìm bệnh nhân và đọc/ghi tệp; nêu thời gian/bộ nhớ dự kiến. Menu chỉ gọi chức năng phần lõi, không tự viết lại cách xếp hàng hoặc chọn phòng.

## 6. Mẫu nội dung và cách duyệt

Mỗi tệp chỉ cần các mục:

1. **Cách làm:** trả lời câu hỏi phần mình bằng lời dễ hiểu.
2. **Các bước xử lý và hình:** mô tả từng bước, có thể dùng chữ hoặc hình vẽ.
3. **Ví dụ và kiểm tra dự kiến:** đầu vào → kết quả mong đợi. Chưa ghi đã chạy đúng khi chưa có code.
4. **Thời gian/bộ nhớ và so sánh:** ghi theo từng thao tác; chỗ chưa chắc đánh dấu để Leader rà.
5. **Nguồn tham khảo và câu hỏi còn vướng.**

Giải thích nhanh: **mã giả** là các bước bằng lời, chưa cần đúng cú pháp C++; **độ phức tạp** mô tả số bước và bộ nhớ tăng ra sao khi có nhiều bệnh nhân; **ca kiểm thử** là một tình huống đầu vào và kết quả mình mong đợi.

Kiểm tra chéo: **C đọc A, A đọc B, B đọc C**; A đối chiếu ví dụ của Leader. Leader duyệt trước khi sang giai đoạn 2. Các phần còn khó được giải thích và bổ sung trước khi duyệt, không bỏ khỏi thiết kế.

Về sau: mỗi người tự viết phần báo cáo tương ứng; C tổng hợp báo cáo, A tổng hợp slide, B chủ trì thực nghiệm. Chưa cần làm những sản phẩm cuối này trong giai đoạn 1. Deadline do Leader tự sắp xếp.

[Tổng quan giai đoạn 1](02_Tong_quan_giai_doan_1.md) · [Quy tắc chung](../README.md#quy-tắc-chính)
