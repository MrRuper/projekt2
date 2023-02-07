#include <iostream>
#include "hash.h"
#include <vector>
#include <cassert>
#include <map>
#include <unordered_set>

namespace jnp1{
    using hash_function_t = uint64_t (*)(uint64_t const*, size_t);
    using vector = std::vector<uint64_t>;

    class Hash{
    public:
        hash_function_t hasher;

        Hash(hash_function_t& some_hash_function){
            this->hasher = some_hash_function;
        }

        uint64_t operator() (std::vector<uint64_t> const& vec) const {
            uint64_t answer = (*(this->hasher))(vec.data(), vec.size());

            return answer;
        }
    };

    std::map<unsigned long int, std::unordered_set<vector, Hash> > keep_all;
    std::map<unsigned long int, hash_function_t> counter_hasher;
    static unsigned long int counter = 0;

    /* Tworzy tablicę haszującą i zwraca jej identyfikator. Parametr
    hash_function jest wskaźnikiem na funkcję haszującą, która daje w wyniku
    liczbę uint64_t i ma kolejno parametry uint64_t const * oraz size_t. */
    unsigned long hash_create(hash_function_t hash_function){
    #ifndef NDEBUG
        std::cerr << "hash_create(" << &hash_function << ")" << std::endl;
    #endif

        if (!hash_function){
            #ifdef DNDEBUG
                std::cerr << "hash_create: invalid pointer (NULL)" << std::endl;
            #endif
            return 0;
        }

        unsigned long int x = counter;

        counter_hasher[x] = hash_function;
        keep_all[x];
        counter++;

        #ifndef DNDEBUG
                std::cerr << "hash_create: hash table #" << x << " created" << std::endl;
        #endif

        return x;
    }

    /* Usuwa tablicę haszującą o identyfikatorze id, o ile ona istnieje.
    W przeciwnym przypadku nic nie robi. */
    void hash_delete(unsigned long id){
        #ifndef DNDEBUG
            std::cerr << "hash_delete(" << id << ")" << std::endl;
        #endif

        std::map<unsigned long int, std::unordered_set<vector, Hash> >::iterator it1;
        std::map<unsigned long int, hash_function_t>::iterator it2;

        it1 = keep_all.find(id);
        it2 = counter_hasher.find(id);

        if (it2 == counter_hasher.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_delete: hash table " << id << " does not exist" << std::endl;
            #endif

            return;
        }
        else {
            #ifndef DNDEBUG
                std::cerr << "hash_delete: hash table #" << id << " deleted";
            #endif

            keep_all.erase(it1);
            counter_hasher.erase(it2);
        }
    }

    /* Daje liczbę ciągów przechowywanych w tablicy haszującej
        o identyfikatorze id lub 0, jeśli taka tablica nie istnieje. */
    size_t hash_size(unsigned long id){
        #ifndef DNDEBUG
            std::cerr << "hash_size(" << id << ")" << std::endl;
        #endif

        std::map<unsigned long int, std::unordered_set<vector, Hash> >::iterator it;

        it = keep_all.find(id);

        if (it == keep_all.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_size: hash table #" << id << "does not exist" << std::endl;
            #endif

            return 0;
        }

        size_t answer = it->second.size();

        #ifndef DNDEBUG
        std::cerr << "hash_size: hash table #" << id << " contains " << answer
                      << " element(s)" << std::endl;
        #endif

        return answer;
    }

    /* "ciag", space */
    void print (uint64_t const* seq, size_t size, unsigned long id){
        if (!seq){
            std::cerr << "NULL, ";
            return;
        }

        std::cerr << "\"";

        for (int i = 0; i < size; i++){
            if (i < size - 1) std::cerr << seq[i] << " ";
            else std::cerr << seq[i];
        }

        std::cerr << "\", " << std::endl;
    }


    /* Wstawia do tablicy haszującej o identyfikatorze id ciąg liczb
    całkowitych seq o długości size. Wynikiem jest informacja, czy operacja
    się powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
    haszującej, jeśli tablica haszująca zawiera już taki ciąg, jeśli
    parametr seq ma wartość NULL lub parametr size ma wartość 0. */
    bool hash_insert(unsigned long id, uint64_t const * seq, size_t size){
        #ifndef DNDEBUG
            std::cerr << "hash_insert:(" << id << ", sequence " ;
            print(seq, size, id);
            std::cerr << size) << std::endl;
        #endif

        if (!seq || size == 0){
            if (!seq){
                #ifndef DNDEBUG
                    std::cerr << "hash_insert: invalid pointer (NULL)" << std::endl;
                #endif
            }
            if (size == 0){
                #ifndef DNDEBUG
                    std::cerr << "hash_insert: invalid size (0)" << std::endl;
                #endif
            }

            return false;
        }

        std::map<unsigned long int, std::unordered_set<vector, Hash> >::iterator it;

        it = keep_all.find(id);

        if (it == keep_all.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_insert: hash table #" << id << " does not exist" << std::endl;
            #endif

            return false;
        }

        std::unordered_set<vector, Hash>::iterator it1;

        std::vector<uint64_t> const vec(seq, seq + size);

        it1 = it->second.find(vec);

        if (it1 != it->second.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_insert: hash table #" << id << ", sequence ";
                print(seq, size, id);
                std::cerr << "was present" << std::endl;
            #endif

            return false;
        }

        it->second.insert(vec);

        #ifndef DNDEBUG
            std::cerr << "hash_insert: hash table #" << id << ", sequence ";
                    print(seq, size, id);
                    std::cerr << "inserted" << std::endl;
        #endif

       return true;
    }

    /* Usuwa z tablicy haszującej o identyfikatorze id ciąg liczb całkowitych
seq o długości size. Wynikiem jest informacja, czy operacja się
        powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
haszującej, jeśli tablica haszująca nie zawiera takiego ciągu,
jeśli parametr seq ma wartość NULL lub parametr size ma wartość 0. */
    bool hash_remove(unsigned long id, uint64_t const * seq, size_t size) {
        #ifndef DNDEBUG
            std::cerr << "hash_remove(" << id << ", ";
            print (seq, size, id);
            std::cerr << ", size)";
        #endif

        if (!seq || size == 0) {
            if (!seq) {
                #ifndef DNDEBUG
                    std::cerr << "hash_remove: invalid pointer (NULL)" << std::endl;
                #endif
            }
            if (size == 0) {
                #ifndef DNDEBUG
                    std::cerr << "hash_remove: invalid size (0)" << std::endl;
                #endif
            }

            return false;
        }

        std::map<unsigned long int, std::unordered_set<vector, Hash> >::iterator it;

        it = keep_all.find(id);

        if (it == keep_all.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_remove: hash_table #" << id << " does not exist" << std::endl;
            #endif

            return false;
        }

        std::unordered_set<vector, Hash>::iterator it1;
        std::vector<uint64_t> const vec(seq, seq + size);

        it1 = it->second.find(vec);

        if (it1 == it->second.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_remove: hash table #" << id << ", sequence ";
                print(seq, size, id);
                std::cerr << "was not present" << std::endl;
            #endif

            return false;
        }

        #ifndef DNDEBUG
            std::cerr << "hash_remove: hash table #" << id << ", sequence ";
                    print(seq, size, id);
                    std::cerr << "removed" << std::endl;
        #endif

        it->second.erase(it1);

        return true;
    }

    /* Jeśli tablica haszująca o identyfikatorze id istnieje i nie jest pusta,
to usuwa z niej wszystkie elementy. W przeciwnym przypadku nic nie robi. */
    void hash_clear(unsigned long id){
        #ifndef DNDEBUG
            std::cerr << "hash_clear(" << id << ")" << std::endl;
        #endif

        std::map<unsigned long int, std::unordered_set<vector, Hash> >::iterator it;

        it = keep_all.find(id);

        if (it == keep_all.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_clear: hash table #" << id << " does not exist" << std::endl;
            #endif

            return;
        }

        if (it->second.empty()){
            #ifndef DNDEBUG
                std::cerr << "hash_clear: hash table #" << id << " was empty" << std::endl;
            #endif

            return;
        }

        #ifndef DNDEBUG
            std::cerr << "hash_clear: hash table #" << id << " cleared" << std::endl;
        #endif

        it->second.clear();
    }

    /* Daje wynik true, jeśli istnieje tablica haszująca o identyfikatorze id
        i zawiera ona ciąg liczb całkowitych seq o długości size. Daje wynik
false w przeciwnym przypadku oraz gdy parametr seq ma wartość NULL lub
        parametr size ma wartość 0. */
    bool hash_test(unsigned long id, uint64_t const * seq, size_t size){
        #ifndef DNDEBUG
            std::cerr << "hash_test(" << id << ", ";
            print(seq, size, id);
            std::cerr << size << std::endl;
        #endif

        if (!seq || size == 0){
            if (!seq){
                #ifndef DNDEBUG
                    std::cerr << "hash_test: invalid pointer (NULL)" << std::endl;
                #endif
            }
            if (size == 0){
                #ifndef DNDEBUG
                    std::cerr << "hash_test: invalid size (0)" << std::endl;
                #endif
            }

            return false;
        }

        auto it = keep_all.find(id);

        if (it == keep_all.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_test: hash table #" << id << " does not exist" << std::endl;
            #endif

            return false;
        }

        std::vector<uint64_t> const vec (seq, seq + size);

        auto it1 = it->second.find(vec);

        if (it1 == it->second.end()){
            #ifndef DNDEBUG
                std::cerr << "hash_test: hash table #" << id << ", sequence ";
                print (seq, size, id);
                std::cerr << "is not present" << std::endl;
            #endif

            return false;
        }

        #ifndef DNDEBUG
            std::cerr << "hash_test: hash table #" << id << ", sequence ";
                    print (seq, size, id);
                    std::cerr << "is present" << std::endl;
        #endif

        return true;
    }

} //end namespace




