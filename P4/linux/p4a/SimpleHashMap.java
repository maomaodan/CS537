///////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  LoadBanlancerMain.java
// File:             SimpleHashMap.java
// Semester:         CS367 Spring 2014
//
// Author:           Wenxuan Mao
// CS Login:         wenxuan
// Lecturer's Name:  Jim Skrentny
// Lab Section:      N/A
//
//////////////////// PAIR PROGRAMMERS COMPLETE THIS SECTION ////////////////////
//                   CHECK ASSIGNMENT PAGE TO see IF PAIR-PROGRAMMING IS ALLOWED
//                   If allowed, learn what PAIR-PROGRAMMING IS, 
//                   choose a partner wisely, and complete this section.
//
// Pair Partner:     N/A
//////////////////////////// 80 columns wide //////////////////////////////////
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

/**
 * This class implements a generic map based on hash tables using chained
 * buckets for collision resolution.
 *
 * <p>A map is a data structure that creates a key-value mapping. Keys are
 * unique in the map. That is, there cannot be more than one value associated
 * with a same key. However, two keys can map to a same value.</p>
 *
 * <p>The <tt>SimpleHashMap</tt> class takes two generic parameters, <tt>K</tt>
 * and <tt>V</tt>, standing for the types of keys and values respectively. Items
 * are stored in a hash table. Hash values are computed from the
 * <tt>hashCode()</tt> method of the <tt>K</tt> type objects.</p>
 *
 * <p>The chained buckets are implemented using Java's <tt>LinkedList</tt>
 * class.  When a hash table is created, its initial table size and maximum
 * load factor is set to <tt>11</tt> and <tt>0.75</tt>. The hash table can hold
 * arbitrarily many key-value pairs and resizes itself whenever it reaches its
 * maximum load factor.</p>
 *
 * <p><tt>null</tt> values are not allowed in <tt>SimpleHashMap</tt> and a
 * NullPointerException is thrown if used. You can assume that <tt>equals()</tt>
 * and <tt>hashCode()</tt> on <tt>K</tt> are defined, and that, for two
 * non-<tt>null</tt> keys <tt>k1</tt> and <tt>k2</tt>, if <tt>k1.equals(k2)</tt>
 * then <tt>k1.hashCode() == k2.hashCode()</tt>. Do not assume that if the hash
 * codes are the same that the keys are equal since collisions are possible.</p>
 */
public class SimpleHashMap<K, V> {


    /**
     * A map entry (key-value pair).
     */
    public static class Entry<K, V> {
        private K key;
        private V value;
        //private fields for the nesting Entry class

        /**
         * Constructs the map entry with the specified key and value.
         */
        public Entry(K k, V v) {
            this.key = k;
            this.value = v;
        }

        /**
         * Returns the key corresponding to this entry.
         *
         * @return the key corresponding to this entry
         */
        public K getKey() {
            return key;
        }

        /**
         * Returns the value corresponding to this entry.
         *
         * @return the value corresponding to this entry
         */
        public V getValue() {
            return value;
        }

        /**
         * Replaces the value corresponding to this entry with the specified
         * value.
         *
         * @param value new value to be stored in this entry
         * @return old value corresponding to the entry
         */
        public V setValue(V value) {
            V oldValue = this.value;
            this.value = value;    //update value field with new value passed in
            return oldValue;
        }
    }
    //private field for the SimpleHashMap class
    private int capacity;           //how big the map is currently
    private double loadFactor;      //ratio of items in the map and its capacity

    private double MAX_LOADFACTOR;  //max ratio of load factor
    private int size;               //current size(numbers of items in the map)

    private int sizePtr;            
    //which size in the pre computed size table to use

    //precomputed table size starting at 11 with 27 other prime numbers used for
    //expanding map size
    private int tableSizes[] = {11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421,
            12487, 24977, 49957, 99923, 199853, 399719, 799441, 1598897,
            3197797, 6395611, 12791227, 25582457, 51164921, 102329861,
            204659729, 409319459, 818638918, 1637277913};

    //array for the hash map
    private LinkedList<Entry<K,V>>[] hashMap;


    /**
     * Constructs an empty hash map with initial capacity <tt>11</tt> and
     * maximum load factor <tt>0.75</tt>.
     **/
    public SimpleHashMap() {
        capacity = 11;
        hashMap = (LinkedList<Entry<K, V>>[]) new LinkedList<?>[capacity];
        MAX_LOADFACTOR = 0.75;
        sizePtr = 0;
        size = 0;
    }

    /**
     * Returns the value to which the specified key is mapped, or null if this
     * map contains no mapping for the key.
     *
     * @param key the key whose associated value is to be returned
     * @return the value to which the specified key is mapped, or <tt>null</tt>
     * if this map contains no mapping for the key
     * @throws NullPointerException if the specified key is <tt>null</tt>
     */
    public V get(Object key) {
        if(key == null)
            throw new NullPointerException();       
        //throw exception if parameter is null

        Entry<K,V> found = getEntry((K)key);    //call private helper method to 
        //get the entry related to key

        if (found != null)      
            return found.getValue();            //return value if found
        else
            return null;                        //or return null
    }

    /**
     * <p>Associates the specified value with the specified key in this map.
     * Neither the key nor the value can be <tt>null</tt>. If the map
     * previously contained a mapping for the key, the old value is replaced.</p>
     *
     * <p>If the load factor of the hash table after the insertion would exceed
     * the maximum load factor <tt>0.75</tt>, then the resizing mechanism is
     * triggered. The size of the table should grow at least by a constant
     * factor in order to ensure the amortized constant complexity. You must also
     * ensure that the new selected size is Prime. After that, all of the mappings 
     * are rehashed to the new table.</p>
     *
     * @param key key with which the specified value is to be associated
     * @param value value to be associated with the specified key
     * @return the previous value associated with <tt>key</tt>, or
     * <tt>null</tt> if there was no mapping for <tt>key</tt>.
     * @throws NullPointerException if the key or value is <tt>null</tt>
     */
    public V put(K key, V value) {
        if (key == null || value == null)
            throw new NullPointerException();//check if parameters are null

        //calculate loadFactor
        loadFactor = ((double)size)/capacity;
        
        //try to find the entry
        Entry<K,V> entryFound = getEntry(key);
        if (entryFound != null)         
            //if found the entry, set new value
        {
            V oldValue = entryFound.getValue();
            entryFound.setValue(value);
            return oldValue;
        }
        else                //if not found
        {
            //check if exceeds load factor and needs to expand
            if (((double)size)/capacity >= MAX_LOADFACTOR)
            {
                sizePtr++;
                List <Entry<K,V>> entries = entries();  //get all entries 

                LinkedList<Entry<K,V>>[] newHashMap = (LinkedList<Entry<K,V>>[]) 
                        new LinkedList<?>[tableSizes[sizePtr]];

                hashMap = newHashMap;
                //make new array for hash map
                capacity = tableSizes[sizePtr];

                //rehash all entries
                for (int i = 0; i < entries.size(); i++)
                {
                    rehash(entries.get(i).getKey(), entries.get(i).getValue());
                }

            }
            //calculate hashcode for the key
            //change to positive by adding table size to the hashcode
            int hashCode = key.hashCode()%capacity;
            if (hashCode < 0)
                hashCode = hashCode + capacity;
            //create linkedlist for null array item
            if (hashMap[hashCode] == null)
            {
                LinkedList<Entry<K,V>> buckets = new LinkedList<Entry<K,V>>();
                hashMap[hashCode] = buckets;
            }
            
            hashMap[hashCode].add(new Entry<K,V>(key,value));
            //add key to list
            size++;
            return null;
        }


    }
    /**
     * called when needs to expand the hash table
     * 
     * put the key and value into the new expanded map with new calculated 
     * hash index
     *
     * @param key key with which the specified value is to be associated
     * @param value value to be associated with the specified key
     * @throws NullPointerException if the key or value is <tt>null</tt>
     */
    private void rehash(K key, V value)
    {

        if (key == null || value == null)
            throw new NullPointerException();//check if parameters are null
        //calculate new hashcode for the key
        int hashCode = key.hashCode()%capacity;
        if (hashCode < 0)
            hashCode = hashCode + capacity; 

        if (hashMap[hashCode] == null)
        {
            hashMap[hashCode] = new LinkedList<Entry<K,V>>();
        }
        hashMap[hashCode].add(new Entry<K,V>(key,value));
        //add entry to new map
    }
    
    /**
     * private helper method to look up entry in the map
     * 
     * looks through the linked list within corresponding hash index
     *
     * @param key key with which the specified value is to be associated
     * @return entry entry found or null if not found
     * @throws NullPointerException if the key or value is <tt>null</tt>
     */
    private Entry<K,V> getEntry(K key)
    {
        if (key == null)
            throw new NullPointerException();       //check if key is null
        //calculate hashcode
        int hashCode = key.hashCode()%capacity;
        if (hashCode < 0)
            hashCode = hashCode + capacity;
        //get the linked list with hashcode
        LinkedList<Entry<K,V>> buckets = hashMap[hashCode];

        //if the list isn't null, then iterate through the list to find 
        //the matching key
        if (buckets != null)
        {
            Iterator<Entry<K,V>> itr = buckets.iterator();
            while (itr.hasNext())
            {
                Entry<K,V> entry1 = itr.next();
                if (entry1!=null)
                {
                    if (entry1.getKey().equals(key))
                        return entry1;
                }

            }
        }
        return null;            //if not found return null
    }



    /**
     * Removes the mapping for the specified key from this map if present. This
     * method does nothing if the key is not in the hash table.
     *
     * @param key key whose mapping is to be removed from the map
     * @return the previous value associated with <tt>key</tt>, or <tt>null</tt>
     * if there was no mapping for <tt>key</tt>.
     * @throws NullPointerException if key is <tt>null</tt>
     */
    public V remove(Object key) {
        if(key == null)
            throw new NullPointerException();       //check if key is null

        //get hashcode of the key
        int hashCode = (key.hashCode())%capacity;
        if (hashCode < 0)
            hashCode = hashCode + capacity;
        
        LinkedList<Entry<K,V>> buckets = hashMap[hashCode];
        //find entry
        Entry<K,V> rm = getEntry((K)key);
        
        if (rm == null)
        {
            System.exit(-1);
        }
        //if entry is found, remove and return
        buckets.remove(rm);
        size--;     //decrement size
        return rm.getValue();
    }

    /**
     * Returns the number of key-value mappings in this map.
     *
     * @return the number of key-value mappings in this map
     */
    public int size() {

        return size;
    }

    /**
     * Returns a list of all the mappings contained in this map. This method
     * will iterate over the hash table and collect all the entries into a new
     * list. If the map is empty, return an empty list (not <tt>null</tt>).
     * The order of entries in the list can be arbitrary.
     *
     * @return a list of mappings in this map
     */
    public List<Entry<K, V>> entries() {
        if (size == 0 || hashMap == null)
            return new ArrayList<Entry<K,V>>();
        
        
        List<Entry<K,V>> rtnList = new ArrayList<Entry<K,V>>();
        
        //iterate through all buckets in the hash map and add all found entries
        //to the list and return
        for (int i = 0; i < hashMap.length; i++)
        {
            if (hashMap[i]!=null)
            {
                LinkedList<Entry<K,V>> buckets = hashMap[i];
                for (int j = 0; j < buckets.size(); j++)

                {
                    if (buckets.get(j) != null)
                        rtnList.add(buckets.get(j));
                }
            }
        }
        return rtnList;
    }

}
