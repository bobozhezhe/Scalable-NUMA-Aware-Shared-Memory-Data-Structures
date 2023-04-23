帮我写程序使用 matplotlib 根据 这个csv 文件的内容来画图。
对不同的container 各画一个方块图，横坐标是loop_num，纵坐标是time(us)。
比如，std::vector 的方块图，以 datatype 的3种operation 为一组，
按照operation 的push_back/read/erase,分为3种颜色。
按照loop_num，从小到大画各个bar
每个相同的loop_num 一组3个operation bar 都比较靠近


| container         | datatype          | operation | loop_num | time(us) |
|-------------------|-------------------|-----------|----------|----------|
| std::forward_list | i                 | push_back | 1000     | 294      |
| std::forward_list | i                 | read      | 1000     | 52       |
| std::forward_list | i                 | erase     | 1000     | 116      |
| std::forward_list | d                 | push_back | 1000     | 130      |
| std::vector       | d                 | push_back | 1000     | 24       |
| std::vector       | d                 | read      | 1000     | 10       |
| std::vector       | d                 | erase     | 1000     | 9        |
| std::vector       | St6vectorIiSaIiEE | push_back | 1000     | 271      |
| std::vector       | St6vectorIiSaIiEE | read      | 1000     | 138      |
| std::vector       | St6vectorIiSaIiEE | erase     | 1000     | 40       |
| std::list         | i                 | push_back | 1000     | 115      |
| std::list         | i                 | read      | 1000     | 21       |
| std::list         | i                 | erase     | 1000     | 51       |
| std::list         | d                 | push_back | 1000     | 84       |
| std::list         | d                 | read      | 1000     | 20       |
| std::forward_list | d                 | push_back | 10000    | 632      |
| std::forward_list | d                 | read      | 10000    | 190      |
| std::forward_list | d                 | erase     | 10000    | 503      |
| std::vector       | i                 | erase     | 10000    | 93       |
| std::vector       | d                 | push_back | 10000    | 184      |
| std::vector       | d                 | read      | 10000    | 249      |
| std::vector       | d                 | erase     | 10000    | 71       |
| std::vector       | St6vectorIiSaIiEE | push_back | 10000    | 3501     |
| std::list         | i                 | erase     | 10000    | 512      |
| std::list         | d                 | push_back | 10000    | 659      |
| std::list         | d                 | read      | 10000    | 177      |
| std::list         | d                 | erase     | 10000    | 883      |
| std::list         | St6vectorIiSaIiEE | push_back | 10000    | 2056     |
| std::list         | St6vectorIiSaIiEE | read      | 10000    | 1081     |
| std::list         | St6vectorIiSaIiEE | erase     | 10000    | 887      |

| std::forward_list | i                 | push_back | 100000   | 7719     |
| std::forward_list | i                 | read      | 100000   | 1657     |
| std::forward_list | i                 | erase     | 100000   | 3885     |
| std::forward_list | d                 | push_back | 100000   | 6845     |
| std::forward_list | d                 | read      | 100000   | 2361     |
| std::forward_list | d                 | erase     | 100000   | 3787     |
| std::forward_list | St6vectorIiSaIiEE | push_back | 100000   | 22997    |
| std::forward_list | St6vectorIiSaIiEE | read      | 100000   | 9198     |
| std::forward_list | St6vectorIiSaIiEE | erase     | 100000   | 5200     |
| std::vector       | i                 | push_back | 100000   | 2044     |
| std::vector       | i                 | read      | 100000   | 1287     |
| std::vector       | i                 | erase     | 100000   | 667      |
| std::vector       | d                 | push_back | 100000   | 1922     |
| std::vector       | d                 | read      | 100000   | 1145     |
| std::vector       | d                 | erase     | 100000   | 605      |
| std::vector       | St6vectorIiSaIiEE | push_back | 100000   | 21483    |
| std::vector       | St6vectorIiSaIiEE | read      | 100000   | 8266     |
| std::vector       | St6vectorIiSaIiEE | erase     | 100000   | 3106     |
| std::list         | i                 | push_back | 100000   | 6318     |
| std::list         | i                 | read      | 100000   | 1925     |
| std::list         | i                 | erase     | 100000   | 6666     |
| std::list         | d                 | push_back | 100000   | 6857     |
| std::list         | d                 | read      | 100000   | 1578     |
| std::list         | d                 | erase     | 100000   | 5469     |
| std::list         | St6vectorIiSaIiEE | push_back | 100000   | 20292    |
| std::list         | St6vectorIiSaIiEE | read      | 100000   | 9632     |
| std::list         | St6vectorIiSaIiEE | erase     | 100000   | 7812     |
| std::forward_list | i                 | push_back | 1000000  | 102344   |
| std::forward_list | i                 | read      | 1000000  | 20420    |
| std::forward_list | i                 | erase     | 1000000  | 48747    |
| std::forward_list | d                 | push_back | 1000000  | 68297    |
| std::forward_list | d                 | read      | 1000000  | 24275    |
| std::forward_list | d                 | erase     | 1000000  | 44812    |
| std::forward_list | St6vectorIiSaIiEE | push_back | 1000000  | 196094   |
| std::forward_list | St6vectorIiSaIiEE | read      | 1000000  | 82698    |
| std::forward_list | St6vectorIiSaIiEE | erase     | 1000000  | 48900    |
| std::vector       | i                 | push_back | 1000000  | 26440    |
| std::vector       | i                 | read      | 1000000  | 10238    |
| std::vector       | i                 | erase     | 1000000  | 7254     |
| std::vector       | d                 | push_back | 1000000  | 15242    |
| std::vector       | d                 | read      | 1000000  | 10224    |
| std::vector       | d                 | erase     | 1000000  | 6933     |
| std::vector       | St6vectorIiSaIiEE | push_back | 1000000  | 185676   |
| std::vector       | St6vectorIiSaIiEE | read      | 1000000  | 81194    |
| std::vector       | St6vectorIiSaIiEE | erase     | 1000000  | 30595    |
| std::list         | i                 | push_back | 1000000  | 62106    |
| std::list         | i                 | read      | 1000000  | 15551    |
| std::list         | i                 | erase     | 1000000  | 46102    |
| std::list         | d                 | push_back | 1000000  | 61633    |
| std::list         | d                 | read      | 1000000  | 16293    |
| std::list         | d                 | erase     | 1000000  | 48635    |
| std::list         | St6vectorIiSaIiEE | push_back | 1000000  | 175000   |
| std::list         | St6vectorIiSaIiEE | read      | 1000000  | 82579    |
| std::list         | St6vectorIiSaIiEE | erase     | 1000000  | 69156    |
