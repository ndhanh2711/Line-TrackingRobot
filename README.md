# Line-Tracking Robot

## Danh mục hình ảnh quy trình thực hiện

### 1. Vẽ mạch dò line và mạch MCU bằng Altium

#### Các linh kiện trong mạch dò line:
1. Điện trở 220 Ohm (8)
2. Quang trở (8)
3. LED trắng (16)  
   - Chức năng: Soi mặt phẳng xe chạy để quang trở tăng khả năng phân biệt màu sắc của line so với mặt đường.
4. Header 10pins (1)
5. Trở băng 10k (1)

![Mạch dò line](image-10.png)

#### Các linh kiện trong mạch MCU:
1. ESP32 DEV KIT V1 (1)
2. Header 2pins (3)
3. Tụ điện:  
   - 100nF (1)  
   - 300nF (1)
4. LM7805 (1)  
   - Chức năng: IC ổn áp, đảm bảo nguồn điện cấp cho toàn mạch ở mức 5V.
5. MOSFET (2)  
   - Chức năng: Điều khiển 2 bánh xe của robot.
6. Điện trở:  
   - 1k  
   - 10k  
   - 150k
7. Diode  
   - Chức năng: Ngăn dòng cảm ứng khi động cơ ngắt đột ngột gây ra có thể làm hỏng MOSFET.
8. Header 10pins (1)

![Mạch MCU](image-11.png)

---

### 2. Sơ đồ đi dây của 2 mạch

![Sơ đồ đi dây 1](image-12.png)
![Sơ đồ đi dây 2](image-13.png)

---

### 3. Quá trình ăn mòn phíp đồng bằng muối ăn mòn chuyên dụng

- **Tips:**  
  - Dây đồng bị mờ sau khi in lên phíp đồng có thể dùng bút lông tô lại.  
  - Dùng nước nóng sẽ giúp quá trình ăn mòn diễn ra nhanh hơn và dây đồng đẹp hơn.

![Quá trình ăn mòn 1](image-14.png)
![Quá trình ăn mòn 2](image-1.png)

---

### 4. Hàn linh kiện lên mạch

![Hàn linh kiện 1](image-2.png)
![Hàn linh kiện 2](image-3.png)
![Hàn linh kiện 3](image-4.png)
![Hàn linh kiện 4](image-5.png)
# Quy trình thực hiện code
## 1 Calib ADC 
- Đọc giá trị của 8 quang trở sao cho trong cùng một môi trường, các giá trị không quá lệch nhau
- -> Từ đó xác định ngưỡng phân biệt giữa Line và Road
## 2 Giảm Duty Cycle -> Giảm tốc độ dộng cơ, tính toán sai số
- Error = Target - Pos
Speed_Left = Base_Speed - PWM
Speed_Right = Base_Speed + PWM
Base_Speed : Tốc độ set ban đầu

- Nếu Error > 0, bánh phải quay nhanh hơn để rẽ phải.

- Nếu Error < 0, bánh trái quay nhanh hơn để rẽ trái
## 3 Mất line
- Xoay một bánh để tìm line
## 4 Thử nghiệm các thông số Ki, Kp, Kd -> Rút ra thông số tối ưu nhất
- PID -> PWM -> Loop
Điều chỉnh thông số PID
Ban đầu, đặt 
- 𝐾p lớn để phản ứng nhanh với sai số.
Sau đó, tăng dần 
- 𝐾i để giảm sai số tích lũy (nếu robot không ổn định).
Sử dụng 
- 𝐾d để làm giảm dao động (overshoot).