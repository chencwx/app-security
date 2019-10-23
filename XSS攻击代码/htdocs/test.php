<?php

$first=$_POST["first"];   //获取name为first的值

$v=$_POST["s"];  //获取name为s的值

$second=$_POST["second"];   //获取name为second的值

if($v=="+"){     //如果是加法运算

echo "The result is".($first+$second);

}elseif($v=="-"){     //如果是减法运算

echo "The result is".($first-$second);

}elseif($v=="*"){     //如果是乘法运算

echo "The result is".($first*$second);

}elseif($v=="%"){    //如果是除法运算

if($second==0){   //如果除数等于0

echo "除数不能是0";    //就输出除数不能是0

}else{

echo "The result is".($first%$second);

}

}elseif($v=="/"){     //如果是模运算

if($second==0){

echo "模不能是0";

}else{

echo "The result is".($first/$second);

}

}

?>