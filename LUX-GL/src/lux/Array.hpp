
namespace lux {

    template <typename T, size_t S>
    class Array {
    private:
        T data[S];
    public:
        const size_t size() const { return S; }

        T& operator[](size_t index) { return data[index]; }
        const T& operator[](size_t index) const { return data[index]; }

        //T* data() { return data; }
        //const T* data() const { return data; }
    };


    template <typename T>
    inline void copyArray(T source[], T dest[], size_t size)
    {
        for (size_t n = 0;n < size;n++)
        {
            dest[n] = source[n];
        }
    }
}