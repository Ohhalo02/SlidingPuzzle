# Sliding Puzzle Game

## Thông tin người làm game
- **Mã sinh viên**: 24021494
- **Họ và tên**: Vũ Minh Hoàng

## Giới thiệu về game:
![image](https://github.com/user-attachments/assets/7146d038-f99f-4716-8cd5-0889f3d4e270)

Sliding Puzzle là một trò chơi giải đố cổ điển trên lưới 3x3, gồm 8 ô số (1-8) và 1 ô trống. Người chơi di chuyển các ô để sắp xếp số theo thứ tự từ 1 đến 8, với ô trống ở góc dưới cùng phải. Game được lập trình bằng C++ với thư viện SDL2, có các tính năng:
- Xáo trộn ngẫu nhiên bàn cờ, đảm bảo luôn giải được.
- Lưu kỷ lục số bước ít nhất ("Best") vào file `highscore.txt`.
- Nút "Give Up" để xem đáp án, hiển thị "You Lose!".
- Giao diện thân thiện với bảng số, số bước di chuyển, và kỷ lục.

Mục tiêu: Hoàn thành trong ít bước nhất để phá kỷ lục!

## Cách chơi
1. **Khởi động game**:
   - Chạy file thực thi, bàn cờ 3x3 xuất hiện với các số được xáo trộn ngẫu nhiên.
2. **Di chuyển ô**:
   - Click vào ô lân cận ô trống (lên, xuống, trái, phải) để hoán đổi vị trí với ô trống.
   - Số bước di chuyển được hiển thị ở "Moves: ".
3. **Hoàn thành**:
   - Sắp xếp các số thành:
     ```
     1 2 3
     4 5 6
     7 8 0
     ```
   - Nếu số bước ít hơn kỷ lục hiện tại, "Best: %d" cập nhật và lưu vào `highscore.txt`.
4. **Tùy chọn**:
   - Nhấn nút **Give Up**: Bàn cờ về trạng thái giải, hiển thị "You Lose!", không cập nhật kỷ lục.
   - Nhấn phím **R**: Reset bàn cờ mới.
   - Nhấn nút **Back** hoặc **Quit**: Thoát game.
5. **Kiểm tra kỷ lục**:
   - "Best: %d" hiển thị kỷ lục từ file `highscore.txt` mỗi khi vào game.

## Các nguồn tham khảo
- **SDL2 Documentation**: Hướng dẫn sử dụng thư viện SDL2 cho giao diện và xử lý sự kiện. [https://wiki.libsdl.org/SDL2/](https://wiki.libsdl.org/SDL2/)
- **Fisher-Yates Shuffle**: Thuật toán xáo trộn ngẫu nhiên để tạo bàn cờ. [https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)
- **8-Puzzle Solvability**: Lý thuyết về tính giải được của bàn cờ 3x3 dựa trên số đảo ngược (inversions). [https://www.cs.princeton.edu/courses/archive/spr10/cos226/assignments/8puzzle.html](https://www.cs.princeton.edu/courses/archive/spr10/cos226/assignments/8puzzle.html)
- **C++ File I/O**: Hướng dẫn đọc/ghi file để lưu kỷ lục. [https://www.cplusplus.com/doc/tutorial/files/](https://www.cplusplus.com/doc/tutorial/files/)
