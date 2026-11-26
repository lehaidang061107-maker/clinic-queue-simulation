# Nháp mục báo cáo: Mô phỏng (Leader)

Nháp cho phần **Mô phỏng** của báo cáo chung. Các con số lấy từ `make test-leader` và lần đo ngày 23/11/2026 (macOS 26.5 arm64, Apple clang 16.0.0). Giai đoạn 6 sẽ chỉnh câu chữ khi ghép báo cáo.

## 1. Mô hình thời gian

Mô phỏng chạy theo từng phút nguyên, bắt đầu từ phút 0. Bệnh viện nhận bệnh trong `[0, closingTime)`, mặc định 0–479. Sau giờ đóng nhận, mô phỏng vẫn chạy tiếp cho đến khi không còn ai chờ hoặc đang khám. Thời gian chờ của một người là `startTime − arrivalTime`, tính bằng phút mô hình, không phải thời gian máy chạy.

## 2. Xử lý một phút (`Simulation::step`)

Thứ tự cố định trong phút `m`:

1. **Hoàn tất:** phòng nào có ca kết thúc đúng phút `m` thì trả phòng về trạng thái rảnh. Người đó chuyển sang `Completed`.
2. **Tiếp nhận:** duyệt kho theo thứ tự `(phút đến, ticket)`, xếp mọi người có phút đến bằng `m` vào phòng đúng khoa có **ít người chờ nhất**; hòa thì chọn mã phòng nhỏ nhất. Người đang khám không tính là đang chờ. Khi bật ưu tiên, mức 0/1 vào heap của phòng, mức 2 vào FIFO; khi tắt ưu tiên, tất cả vào FIFO.
3. **Ghi nhận hàng chờ:** cập nhật số chờ lớn nhất theo phòng và toàn hệ thống.
4. **Gọi khám:** mỗi phòng rảnh lấy người tiếp theo: heap trước (mức → phút đến → ticket), rồi mới đến FIFO. Ca đang khám không bao giờ bị ngắt.
5. **Ghi nhận lại hàng chờ** sau khi gọi, rồi sang phút `m + 1`.

**Tiếp nhận tay từ menu** (`admit`): ticket = ticket lớn nhất + 1, phút đến = phút sắp xử lý. Người mới được xếp phòng ngay bằng đúng hàm của bước 2, nên kết quả giống hệt khi người đó có sẵn trong tệp. Xếp phòng trước bước hoàn tất của phút đó vẫn cho cùng phòng, vì bước hoàn tất không làm đổi số người chờ.

**Kết thúc ngày:** `isFinished()` đúng khi đã qua giờ đóng nhận, mọi người đã `Completed` và không phòng nào còn bận hay còn hàng. **Phút kết thúc ngày** (`lastFinishMinute()`) là `finishTime` lớn nhất trong các người đã khám xong, bằng −1 nếu chưa ai xong. Khi ngày đã kết thúc và có bệnh nhân, giá trị này bằng `lastProcessedMinute()`.

## 3. Độ phức tạp

Đã cập nhật ở giai đoạn 5 (con trỏ duyệt người đến, bộ đếm hoàn tất): xem [nháp Độ phức tạp và hiệu năng](../Giai%20Doan%205/G5_Leader_Do_phuc_tap_nhap.md). Bản G4 (duyệt lại cả kho mỗi phút, O(T·(N + R) + N·(R + log q))) không còn đúng với mã hiện tại.

## 4. Vì sao 2 phòng bật/tắt ưu tiên có lúc cho kết quả giống hệt

Định nghĩa: một lần gọi khám **vượt** một người nếu, lúc gọi, cùng phòng còn một người đến trước (theo phút đến rồi ticket) mà chưa được gọi.

- Tắt ưu tiên thì mọi phòng phục vụ đúng thứ tự (phút đến, ticket), nên không bao giờ có vượt.
- Nếu suốt cả ngày ở chế độ bật ưu tiên cũng không có lần vượt nào, thì mỗi phòng gọi đúng người mà chế độ tắt ưu tiên sẽ gọi. Chọn phòng chỉ phụ thuộc số người chờ, nên trạng thái hai chế độ giống nhau ở mọi phút. Suy ra lịch khám trùng từng người.

`overtakeExplanation` trong `tests/test_leader.cpp` đếm số cặp vượt (lần gọi, người bị vượt), chạy cả hai chế độ rồi so lịch bắt đầu của từng người:

| Số người (seed 2026) | Phòng/khoa | Số cặp vượt | Lần gọi khi phòng có cả heap và FIFO | Lịch bật = lịch tắt ưu tiên |
|---|---|---|---|---|
| 120 | 1 | 38 | 30 | Không |
| 120 | 2 | 0 | 0 | **Có** |
| 150 | 1 | 267 | 70 | Không |
| 150 | 2 | 0 | 0 | **Có** |
| 180 | 1 | 695 | 99 | Không |
| 180 | 2 | 5 | 6 | Không |

Kết luận: số liệu trùng nhau là đúng, không phải lỗi. Khi đủ phòng, hàng của mỗi phòng hầu như không bao giờ có hai người cùng chờ mà người đến sau lại được ưu tiên hơn, nên ưu tiên không có dịp phát huy. Ưu tiên chỉ tạo khác biệt khi phòng bị dồn hàng. Test khẳng định cả hai chiều: không có vượt thì lịch trùng, lịch trùng thì không có vượt.

Hai cột không phải lúc nào cũng bằng nhau. Một lần gọi có thể diễn ra khi phòng có cả heap và FIFO mà vẫn không vượt ai, nếu người ở heap đến trước người ở FIFO. Một lần gọi cũng có thể vượt nhiều người cùng lúc.
