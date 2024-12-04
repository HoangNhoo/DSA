# Informal Description

Ý tưởng cơ bản của thuật toán là ta thu được nhiều thông tin hơn khi so khớp mẫu từ phía bên phải thay vì từ bên trái. Hãy tưởng tượng rằng mẫu (pat) được đặt lên trên phần đầu bên trái của chuỗi (string) sao cho các ký tự đầu tiên của hai chuỗi được căn thẳng hàng. Hãy xem xét điều chúng ta học được nếu ta lấy ký tự thứ patlen của chuỗi, được gọi là char. Đây là ký tự thẳng hàng với ký tự cuối cùng của mẫu (pat).

+ Quan sát 1: Nếu char được biết là không xuất hiện trong pat, thì ta biết rằng ta không cần xem xét khả năng xảy ra của pat tại các vị trí 1, 2, ..., hay patlen trong chuỗi string. Lý do là để có một kết quả khớp, char phải là một ký tự của pat.
+ Quan sát 2: Tổng quát hơn, nếu lần xuất hiện cuối cùng (bên phải) của char trong pat là cách ký tự cuối của pat một khoảng là delta₁, thì ta biết rằng ta có thể trượt pat xuống delta₁ vị trí mà không cần kiểm tra các phép so khớp. Lý do là nếu ta trượt pat xuống ít hơn delta₁, lần xuất hiện của char trong string sẽ thẳng hàng với một ký tự nào đó của pat mà nó không thể khớp (vì một kết quả khớp sẽ yêu cầu char xuất hiện trong pat ở bên phải nhất).

Do đó, trừ khi char khớp với ký tự cuối cùng của pat, ta có thể dịch xuống delta₁ ký tự trong chuỗi string mà không cần xem xét các ký tự đã bỏ qua; delta₁ là một hàm phụ thuộc vào ký tự char thu được từ string. Nếu char không xuất hiện trong pat, thì delta₁ là độ dài của patlen. Nếu char xuất hiện trong pat, thì delta₁ là **hiệu số giữa patlen và vị trí xuất hiện cuối cùng của char trong pat**.

Bây giờ, giả sử char khớp với ký tự cuối cùng của pat. Khi đó, ta phải xác định xem ký tự trước đó trong string có khớp với ký tự thứ hai từ cuối trong pat không. Nếu có, ta tiếp tục kiểm tra lùi lại cho đến khi toàn bộ pat được so khớp (và do đó tìm được một kết quả khớp), hoặc xảy ra sai lệch tại một ký tự mới sau khi đã khớp được m ký tự của pat.

Trong trường hợp sau, ta muốn dịch pat xuống để xem xét khả năng so khớp tiếp theo. Tất nhiên, ta muốn dịch xa nhất có thể.

+ Quan sát 3(a):

Nếu ký tự không khớp char và delta₁ (khoảng cách từ ký tự cuối cùng trong mẫu so khớp pat đến vị trí xuất hiện cuối cùng của char trong pat) đã được biết, chúng ta có thể trượt pat xuống k ký tự sao cho hai lần xuất hiện đã biết của char trong pat được căn chỉnh. Sau đó, ta sẽ kiểm tra ký tự của chuỗi string tương ứng với ký tự cuối cùng của pat.

Khoảng cách k phụ thuộc vào vị trí mà char xuất hiện trong pat.
Nếu lần xuất hiện cuối của char trong pat nằm ở bên phải vị trí mismatch, khi đó, delta₁ không cần dịch chuyển thêm mà có thể giữ nguyên k = 1.
Tuy nhiên, nếu lần xuất hiện cuối của char nằm ở bên trái vị trí mismatch, chúng ta có thể dịch thêm khoảng delta(char) - m (m là số ký tự đã so khớp).

+ Quan sát 3(b):

Giả sử rằng chúng ta đã khớp được m ký tự cuối của pat trong string, nhưng một mismatch xảy ra sau đó. Khi đó:

Chúng ta cần trượt pat sao cho một "sự xuất hiện khả dĩ" (plausible reoccurrence) của đoạn subpat cuối cùng (là m ký tự cuối của pat) được căn chỉnh với vị trí phù hợp nhất trong chuỗi.
Khoảng cách dịch chuyển k được tính dựa trên vị trí của đoạn subpat trong pat, đồng thời không để phần cuối của subpat rơi ra khỏi phạm vi pat.
Sau khi dịch chuyển k ký tự, ta sẽ tiếp tục kiểm tra từ ký tự cuối của pat. Khoảng cách này được gọi là delta₂, được định nghĩa như sau:

delta₂ = (pattern length - j), với j là vị trí xuất hiện gần nhất của subpat trong pat.
