# 1. 컴파일러 및 컴파일 옵션 설정
CC = gcc
CFLAGS = -Wall -Wextra -g

# 2. 타겟 실행 파일 이름
TARGET = scheduler_sim

# 3. 소스 파일 및 오브젝트 파일 목록 (여기에 .c 파일만 나열하면 됩니다)
# SRCS = simulation.c data_struct.c sched.c test_gen.c
SRCS = main.c simulation.c data_struct.c sched.c test_gen.c draw_gantt.c
OBJS = $(SRCS:.c=.o)

# 4. 기본 타겟
all: $(TARGET)

# 5. 링크: 오브젝트 파일들을 묶어서 최종 실행 파일 생성
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 6. 컴파일: 모든 .c 파일을 대응하는 .o 파일로 컴파일 (패턴 룰)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 7. 헤더 파일 의존성: 모든 오브젝트 파일은 data_struct.h가 변경되면 다시 빌드됨
$(OBJS): data_struct.h

# 8. 정리 및 실행 명령어
clean:
	rm -rf output_gantt
	rm -rf output_testcase
	mkdir output_gantt
	mkdir output_gantt/fcfs
	mkdir output_gantt/rr
	mkdir output_gantt/sjf
	mkdir output_gantt/pre_sjf
	mkdir output_gantt/prior
	mkdir output_gantt/pre_prior
	mkdir output_testcase
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)