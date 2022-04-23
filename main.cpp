#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

bool search_frames(int page, const int frame[], int frame_size)
{
    for (int i = 0; i < frame_size; ++i)
    {
        if (page == frame[i])
            return true;
    }
    return false;
}

void print_frames(bool is_frame_full, int frame[], int frame_size, int frame_capacity)
{
    for (int i = 0; i < (is_frame_full ? frame_capacity : frame_size); ++i)
        printf("%02d ", frame[i]);
    cout << "\n";
}

int OPTIMAL(int frame_capacity, vector<int> page_stream)
{
    int frame[frame_capacity], total_faults = 0, frame_size = 0;
    bool is_frame_full = false;
    for (int i = 0; i < page_stream.size(); i++)
    {
        if (search_frames(page_stream[i], frame, frame_capacity))
        {
            printf("%02d     ", page_stream[i]);
            print_frames(is_frame_full, frame, frame_size, frame_capacity);
        } else
        {
            if (is_frame_full)
            {
                unordered_set<int> page_future;
                bool exit_flag = false;
                for (int j = i + 1; j < page_stream.size(); ++j)
                {
                    for (int k = 0; k < frame_capacity; ++k)
                    {
                        if (page_future.find(page_stream[j]) == page_future.end() && page_stream[j] == frame[k])
                        {
                            page_future.insert(page_stream[j]);
                            if (page_future.size() == frame_capacity - 1)
                                exit_flag = true;
                            break;
                        }
                    }
                    if (exit_flag)
                        break;
                }
                for (int j = 0; j < frame_capacity; ++j)
                {
                    if (page_future.find(frame[j]) == page_future.end())
                    {
                        frame[j] = page_stream[i];
                        break;
                    }
                }
                printf("%02d F   ", page_stream[i]);
                total_faults++;
            } else
            {
                frame[frame_size] = page_stream[i];
                printf("%02d     ", page_stream[i]);
                frame_size++;
            }
            print_frames(is_frame_full, frame, frame_size, frame_capacity);
            if (frame_size == frame_capacity && !is_frame_full)
                is_frame_full = true;
        }
    }
    return total_faults;
}

int FIFO(int frame_capacity, const vector<int> &page_stream)
{
    int frame[frame_capacity], total_faults = 0, frame_index = 0;
    bool is_frame_full = false;
    for (int page: page_stream)
    {
        if (search_frames(page, frame, frame_capacity))
        {
            printf("%02d     ", page);
            print_frames(is_frame_full, frame, frame_index, frame_capacity);
        } else
        {
            frame[frame_index++] = page;
            if (is_frame_full)
            {
                printf("%02d F   ", page);
                total_faults++;
            } else
                printf("%02d     ", page);

            print_frames(is_frame_full, frame, frame_index, frame_capacity);
            if (frame_index == frame_capacity)
            {
                if (!is_frame_full)
                    is_frame_full = true;
                frame_index = 0;
            }
        }
    }
    return total_faults;
}

int LRU(int frame_capacity, const vector<int> &page_stream)
{
    int frame[frame_capacity], total_faults = 0, frame_size = 0;
    bool is_frame_full = false;
    for (int i = 0; i < page_stream.size(); i++)
    {
        if (search_frames(page_stream[i], frame, frame_capacity))
        {
            printf("%02d     ", page_stream[i]);
            print_frames(is_frame_full, frame, frame_size, frame_capacity);
        } else
        {
            if (is_frame_full)
            {
                unordered_set<int> page_history;
                int j = i - 1;
                while (page_history.size() < frame_capacity - 1 && j >= 0)
                {
                    if (page_history.find(page_stream[j]) == page_history.end())
                        page_history.insert(page_stream[j]);
                    j--;
                }
                for (j = 0; j < frame_capacity; ++j)
                {
                    if (page_history.find(frame[j]) == page_history.end())
                    {
                        frame[j] = page_stream[i];
                        break;
                    }
                }
                printf("%02d F   ", page_stream[i]);
                total_faults++;
            } else
            {
                frame[frame_size] = page_stream[i];
                printf("%02d     ", page_stream[i]);
                frame_size++;
            }
            print_frames(is_frame_full, frame, frame_size, frame_capacity);
            if (frame_size == frame_capacity && !is_frame_full)
                is_frame_full = true;
        }
    }
    return total_faults;
}

int CLOCK(int frame_capacity, const vector<int> &page_stream)
{
    int frame[frame_capacity], total_faults = 0, frame_pointer = 0, frame_size = 0;
    bool is_frame_full = false, use_bit[frame_capacity];
    for (int i = 0; i < frame_capacity; ++i)
        use_bit[i] = false;
    for (int page: page_stream)
    {
        if (search_frames(page, frame, frame_capacity))
        {
            for (int j = 0; j < frame_capacity; ++j)
            {
                if (frame[j] == page)
                {
                    use_bit[j] = true;
                    printf("%02d     ", page);
                    print_frames(is_frame_full, frame, frame_size, frame_capacity);
                    break;
                }
            }
        } else
        {
            if (is_frame_full)
            {
                while (true)
                {
                    if (use_bit[frame_pointer])
                    {
                        use_bit[frame_pointer] = false;
                        frame_pointer = (frame_pointer + 1) % frame_capacity;
                    } else
                    {
                        frame[frame_pointer] = page;
                        use_bit[frame_pointer] = true;
                        frame_pointer = (frame_pointer + 1) % frame_capacity;
                        printf("%02d F   ", page);
                        total_faults++;
                        break;
                    }
                }
            } else
            {
                use_bit[frame_size] = true;
                frame[frame_size] = page;
                printf("%02d     ", page);
                frame_size++;
            }
            print_frames(is_frame_full, frame, frame_size, frame_capacity);
            if (frame_size == frame_capacity && !is_frame_full)
                is_frame_full = true;
        }
    }
    return total_faults;
}

int main()
{
    int frame_capacity, total_faults = 0;
    string rep_policy;
    cin >> frame_capacity >> rep_policy;
    int new_page;
    vector<int> page_stream;
    while (true)
    {
        cin >> new_page;
        if (new_page != -1)
            page_stream.push_back(new_page);
        else
            break;
    }
    cout << "Replacement Policy = " << rep_policy << "\n";
    cout << "-------------------------------------\n";
    cout << "Page   Content of Frames\n";
    cout << "----   -----------------\n";
    if (rep_policy == "OPTIMAL")
        total_faults = OPTIMAL(frame_capacity, page_stream);
    else if (rep_policy == "FIFO")
        total_faults = FIFO(frame_capacity, page_stream);
    else if (rep_policy == "LRU")
        total_faults = LRU(frame_capacity, page_stream);
    else if (rep_policy == "CLOCK")
        total_faults = CLOCK(frame_capacity, page_stream);
    cout << "-------------------------------------\n";
    cout << "Number of page faults = " << total_faults << "\n";
    return 0;
}
