# Quy trình Git cho thành viên

Hướng dẫn thành viên nhận việc, tạo branch, phát triển chức năng và gửi Pull Request (PR) để PM review và merge.

**Nguyên tắc: 1 công việc = 1 branch = 1 PR.**

## 1. Nhận việc

Nhận việc từ PM hoặc thống nhất trong nhóm. Xác định rõ nội dung cần làm trước khi bắt đầu.

Ví dụ:

```text
Làm API đăng nhập
```

## 2. Tạo branch

Luôn tạo branch từ `develop` đã cập nhật:

```bash
git checkout develop
git pull origin develop
git checkout -b feature/login-api
```

Đặt tên branch theo chức năng, viết thường và phân cách bằng dấu `-`. Dùng `feature/` cho chức năng mới và `fix/` cho sửa lỗi.

```text
feature/login-api
feature/user-profile
fix/login-validation
```

## 3. Code và commit

Làm đúng phạm vi công việc đã nhận. Kiểm tra thay đổi rồi commit:

```bash
git status
git add <file-can-commit>
git commit -m "feat: add login API"
```

Thay `<file-can-commit>` bằng đường dẫn file cần đưa vào commit.

**Không commit thông tin nhạy cảm như file `.env`, mật khẩu, API keys hoặc secrets.**

## 4. Push

Đẩy branch lên remote:

```bash
git push -u origin feature/login-api
```

## 5. Tạo Pull Request

Tạo PR từ branch của mình vào `develop`:

```text
feature/login-api → develop
```

Tên PR ghi rõ nội dung, ví dụ:

```text
Add login API
```

Mô tả PR ngắn gọn theo mẫu:

```markdown
## Changes
- Thêm API đăng nhập
- Thêm kiểm tra dữ liệu đầu vào

## Testing
- Đăng nhập thành công với tài khoản hợp lệ
- Trả lỗi khi sai mật khẩu
```

Ghi đúng những gì đã thay đổi và thực sự kiểm tra.

## 6. Sau khi tạo PR

Gửi link PR cho PM và chờ review, merge.

Nếu PM yêu cầu sửa, tiếp tục trên cùng branch:

```text
Sửa code → Commit → Push → PR tự cập nhật
```

Không cần tạo PR mới cho các chỉnh sửa của cùng công việc.

## 7. Nếu có merge conflict

**Không cần tự xử lý.** Báo PM và gửi kèm link PR:

```text
PR login-api đang conflict, nhờ PM xử lý giúp.
```

## 8. Checklist cho thành viên

- [ ] Nhận việc và xác định phạm vi.
- [ ] Cập nhật `develop` và tạo branch riêng.
- [ ] Code, kiểm tra thay đổi và commit.
- [ ] Push branch lên remote.
- [ ] Tạo PR vào `develop` và gửi PM review.
- [ ] Sửa theo góp ý trên cùng branch nếu cần.
- [ ] PM merge PR và công việc hoàn thành.

### Chỉ cần nhớ

```text
1 công việc = 1 branch = 1 PR
```
