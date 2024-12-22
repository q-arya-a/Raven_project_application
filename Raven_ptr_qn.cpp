template <typename T>
class my_unique_ptr{
    private:
        //raw pointer
        T* ptr = nullptr;

    public:
        //constructor with no arguments
        my_unique_ptr() : ptr(nullptr) {}

        //constructor
        my_unique_ptr(T* ptr) : ptr(ptr) {}

        /*Remove copy semantics*/

        my_unique_ptr(const my_unique_ptr & obj) = delete;
        //copy constructor is deleted

        my_unique_ptr& operator=(const my_unique_ptr & obj) = delete;
        //copy assignment is deleted

        //move constructor
        my_unique_ptr(my_unique_ptr&& other){
            //Transfer ownership of memory pointed by other
            ptr = other.ptr;
            other.ptr = nullptr;
        }

        //move assignment operator
        my_unique_ptr& operator=(my_unique_ptr&& other){
            //prevent self assignment
            if(this != other){
                delete ptr;     //free already existing resource
                ptr = other.ptr;      //Transfer the ownership
                other.ptr = nullptr;
            }
            return *this;
        }

        //Overload dereference and arrow operators
        T& operator*() const {return *ptr; }
        T* operator->() const {return ptr; }

        // check if the pointer is not null
        explicit operator bool() const (return ptr != nullptr; )

        //release ownership of the managed object
        T* release() {
            T* temp = ptr;
            ptr = nullptr;
            return temp;
        }

        //replace the managed object
        void reset(T* other = nullptr){
            delete ptr;
            ptr = other;
        }

};