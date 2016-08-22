# VNG_bot_battle

VNG 11+1 Bot Battle Tournament là cuộc thi điều khiển và lập trình robot với hình thức thi đấu đối kháng, nhằm nâng cao tư duy lập trình robot tự động của Clickers.

Lấy format quen thuộc từ một số cuộc thi robot thế giới, VNG 11+1 Bot Battle Tournament  là sân chơi thú vị để những lập trình viên VNG có thể giao lưu và học hỏi thêm về xu hướng tự động hóa, IoT (Internet of Things).

Với mục đích phổ biến rộng rãi cho toàn thể những thành viên VNG về xu hướng IoT hiện đang rất được giới công nghệ quốc tế chú trọng, VNG 11+1 Bot Battle Tournament  như một cú hích lớn, thúc đẩy dân công nghệ của VNG tự tin hơn, mày mò khám phá nhiều hơn trên lĩnh vực robot tự động hoá cực kì thú vị nhưng không hề dễ dàng này.

Toàn bộ phần cứng của robot sẽ do các kỹ sư của IoTLab tự sản xuất, phần lập trình sẽ là nơi để các đội tham gia trổ tài sáng tạo. Như vậy, cuộc thi sẽ là một minh chứng sống động cho năng lực sáng tạo cả phần mềm lẫn phần cứng của các kỹ sư VNG.

Thể thức thi đấu và luật chơi VNG 11+1 Bot Battle Tournament

Hình thức thi đấu: đối kháng, loại trực tiếp

Quy định về robot: được build thêm đồ thoải mái, với điều kiện:

+ Trọng lượng tối đa: 1kg

+ Kích thước tối đa: 25x25cm

Sàn thi đấu:
Sandau

Hình tròn đường kính 1m, đặt cao hơn mặt đất 10cm. Sàn bằng gỗ, sơn trắng, có vạch đen bao quanh chiều rộng 7cm

Cách tính điểm: Mỗi trận đấu gồm 3 ván, tính điểm theo thể thức BO3 (đội giành chiến thắng hai ván sẽ là đội giành chiến thắng chung cuộc).

Luật chơi:
HFGHJGJG

Robot đặt hướng ngang cạnh vào nhau giữa sân, quay đầu ngược nhau, cách nhau 5cm

+ Ván 1, robot đặt hướng ngang cạnh vào nhau giữa sân, quay đầu ngược nhau, cách nhau 5cm

+ Ván 2 trở đi, robot nào thua ván trước thì được quyền đặt vị trí sau, tuy nhiên phải xa hơn robot kia tối khoảng cách tối thiểu 20cm

+ Trong quá trình thi đấu, robot nào rớt khỏi sàn đấu coi như thua, chạm mặt đất cũng coi như thua.

+ Nếu sau 3 ván kết quả 2 đội là hoà thì đấu thêm 1 ván phụ cho tới khi thắng, hoặc do sự quyết định của trọng tài.

+ Trong ván đấu, nếu 2 robot đối đầu trực diện mà không dịch chuyển trong 10 giây, trọng tài sẽ can thiệp.

+ Robot nếu trục trặc kĩ thuật trong quá trình thi đấu sẽ bị loại ngay lập tức mà không cần rơi khỏi sàn đấu.

+ Nếu cả 2 robot cùng trục trặc trong quá trình thi đấu thì cho phép 2 đội có 5 phút sửa chữa. Sau đó tiếp tục trận đấu. Nếu sau 5 phút cả 2 đều không sửa chữa được thì loại cả 2.

Thời gian thi đấu:

+ Mỗi ván thi đấu tối đa 5 phút.

+ Sau 5 phút, nếu không có đội thua thì ván đó được coi như hòa.

Những chiến binh trong VNG11+1 Bot Battle Tournament được cấu tạo như thế nào?

Về phần cứng, cấu tạo của bot bao gồm:
Zalo_ScreenShot_16_8_2016_1355832

2 Bánh xe chủ động; Công tắc nguồn; Jack sạc pin 2 cell
Zalo_ScreenShot_16_8_2016_144996

3 Cảm biến quang nằm dưới đáy bot, chủ yếu để dò 2 màu trắng/đen; 1 Bánh xe bị động
Zalo_ScreenShot_16_8_2016_146371

Cảm biến phía trước đầu xe, đo được khoảng cách tương đối, để dò tìm đối thủ.
Zalo_ScreenShot_16_8_2016_143797

Bo mạch; Mạch nạp

Lưu ý: Khi cắm Pin lipo vào sẽ có 2 đèn chuyển sang màu đỏ, đó là 2 cell đang được sạc và khi chuyển sang màu xanh là Pin đã được sạc đầy. Các đội lưu ý nên sạc pin thường xuyên vì nếu điện áp 2 cell xuống dưới 6.6V sẽ rất dễ hư pin.Zalo_ScreenShot_16_8_2016_149890Về phần mềm của bot, sẽ do chính các đội chơi lập trình online.

Bước 1: Truy cập vào trang https://developer.mbed.org/account/signup/?next=%2F và tạo 1 tài khoản

Zalo_ScreenShot_16_8_2016_131441

Bước 2: Nhấp vào tab complier vào trình soạn thảo và build code.

Zalo_ScreenShot_16_8_2016_1314769

Bước 3: Nhấp vào New chọn flatform là Nordic nrf51822 (nếu lần đầu chưa có, bạn có thể add vào theo hướng dẫn của ứng dụng. Tiếp theo chọn 1 template, ở đây có thể chọn BLE Beacon demo rồi đặt tên cho project.

Zalo_ScreenShot_16_8_2016_1315809

Bước 4: Mở file main.cpp của project và pass nội dung file main của bot vào, sau đó click vào ô compiler và đợi mbed build xong nó sẽ tự download file hex về máy tính.

Zalo_ScreenShot_16_8_2016_1315936

Bước 5: Tắt nguồn Sumo bot và gắn mạch nạp vào bo Vbluno rồi kết nối với máy tính, nếu kết nối thành công sẽ hiện lên 1 ổ đĩa có tên là MBED. Copy file hex đã build từ trước và pass vào ở đĩa MBED, nếu nhìn thấy led trên mạch nạp chớp tắt là đang trong quá trình nap, sau đó led ngừng tắt là quá trình nạp đã thành công, sau đó tháo mạch nạp và test sumo bot.

Zalo_ScreenShot_16_8_2016_1315968

Lưu ý: trong quá trình kết nối mạch nạp với máy tính nếu không hiện lên ổ đĩa ta nên kiểm tra lại kết nối bằng cách tháo ra và cắm lại.
