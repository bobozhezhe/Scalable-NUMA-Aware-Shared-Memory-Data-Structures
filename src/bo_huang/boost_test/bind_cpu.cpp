#include <sched.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(0, &cpu_mask); // 设置进程绑定到CPU0

    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask) != 0) {
        perror("sched_setaffinity");
        return -1;
    }

    // 打印当前进程绑定的CPU编号
    printf("Process is running on CPU %d\n", sched_getcpu());

    // 其他代码
    return 0;
}

