<?php

$first=$_POST["first"];   //��ȡnameΪfirst��ֵ

$v=$_POST["s"];  //��ȡnameΪs��ֵ

$second=$_POST["second"];   //��ȡnameΪsecond��ֵ

if($v=="+"){     //����Ǽӷ�����

echo "The result is".($first+$second);

}elseif($v=="-"){     //����Ǽ�������

echo "The result is".($first-$second);

}elseif($v=="*"){     //����ǳ˷�����

echo "The result is".($first*$second);

}elseif($v=="%"){    //����ǳ�������

if($second==0){   //�����������0

echo "����������0";    //���������������0

}else{

echo "The result is".($first%$second);

}

}elseif($v=="/"){     //�����ģ����

if($second==0){

echo "ģ������0";

}else{

echo "The result is".($first/$second);

}

}

?>