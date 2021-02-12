module full_adder_1bit(x, y, c, cout, summ);  wire _00_;  wire _01_;  wire _02_;  wire _03_;  wire _04_;  wire _05_;  wire _06_;  input c;  output cout;  output summ;  input x;  input y;  AND2X2 _07_ (    .A(x),    .B(y),    .Y(_03_)  );  NOR2X1 _08_ (    .A(x),    .B(y),    .Y(_04_)  );  OAI21X1 _09_ (    .A(_04_),    .B(_03_),    .C(c),    .Y(_05_)  );  INVX1 _10_ (    .A(c),    .Y(_06_)  );  NAND2X1 _11_ (    .A(x),    .B(y),    .Y(_00_)  );  OR2X2 _12_ (    .A(x),    .B(y),    .Y(_01_)  );  NAND3X1 _13_ (    .A(_06_),    .B(_00_),    .C(_01_),    .Y(_02_)  );  NAND2X1 _14_ (    .A(_05_),    .B(_02_),    .Y(summ)  );  OAI21X1 _15_ (    .A(_06_),    .B(_04_),    .C(_00_),    .Y(cout)  );endmodule
