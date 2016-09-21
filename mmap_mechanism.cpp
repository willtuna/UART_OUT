class fixed_size // acutally this is not queue
{
public:
  void push(const drone_data&);
  void fetch(drone_data*);
  int have_new_data();

private:
  drone_data data;
  int newdata=0;
};

void fixed_size_queue::push(const drone_data& d_data)
{
  drone_data = d_data;
  newdata = 1;
}
void fixed_size_queue::fetch(drone_data* d_data)
{
    *d_data = drone_data;
    newdata =0;
}

int fixed_size_queue::have_new_data()
{
  return newdata;
}
