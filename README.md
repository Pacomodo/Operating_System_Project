## Operating_System_Project
2024 가을 학기 운영체제 수업에서 구현했던 세 가지 미니 프로젝트입니다.

1번 프로젝트는 기존 리눅스 커널을 수정하여 간단한 system call을 하나 추가하는 프로젝트입니다.\
리눅스 커널은 Ubuntu 18.04.2 LTS distribution에서 나온 것을 사용했습니다.\
스택에 숫자를 넣고 빼는 system call을 구현하였고, 이를 이용해 랜덤 정수를 최소 3번이상 각각 넣었다 빼는 작업을 반복했습니다.\
dmesg로 커널 로그를 확인한 결과는 result.txt에 있습니다.

2번 프로젝트는 kernel space에 가상 CPU를 구현하여 두가지 process scheduling policies를 분석하는 프로젝트입니다.\
1번 프로젝트의 연장선으로 이어진 프로젝트이고, FCFS와 SJF를 각각 구현하여 분석하였습니다.

3번 프로젝트는 procfs를 활용해 물리,가상,공유 메모리 사용의 변화를 분석하는 C 프로그램을 구현하여, Firefox, 동영상 재생 등 새로운 user process를 실행할 때의 메모리 사용량의 변화를 관찰하고 분석하는 프로젝트입니다.\
C 프로그램을 구현하였고 이를 통해 다양한 process실행 시 변화량을 관측하였습니다.