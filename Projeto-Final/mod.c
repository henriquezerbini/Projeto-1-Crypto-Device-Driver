#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/crypto.h>
#include <linux/slab.h>
#include <linux/ceph/ceph_debug.h>
#include <linux/random.h>
#include <linux/list.h>

#include <crypto/hash.h>
#include <crypto/sha.h>


#include <linux/err.h>
#include <linux/errno.h>
#include <linux/scatterlist.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <crypto/aes.h>
#include <crypto/skcipher.h>
#include <linux/key-type.h>

#include <keys/ceph-type.h>
#include <keys/user-type.h>



#define TAMCHAVE 16
#define  DEVICE_NAME "ebbchar"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "ebb"        ///< The device class -- this is a character device driver



#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')


#define TAMANHOCHAVE 16

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("GRUPO JB");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the BBB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {"teste"};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer

static char *mystring = "blah";
static char final[TAMCHAVE];


module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");


// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};






struct tcrypt_result {
    struct completion completion;
    int err;
};

/* tie all data structures together */
struct skcipher_def {
    struct scatterlist sg;
    struct crypto_skcipher *tfm;
    struct skcipher_request *req;
    struct tcrypt_result result;
};

/* Callback function */
static void test_skcipher_cb(struct crypto_async_request *req, int error)
{
    struct tcrypt_result *result = req->data;
    if (error == -EINPROGRESS)
        return;
    result->err = error;
    complete(&result->completion);
    pr_info("Encryption finished successfully\n");
}



static struct skcipher_def sk;
static struct crypto_skcipher *skcipher = NULL;
static struct skcipher_request *req = NULL;
static char *scratchpad = NULL,  *testeC = NULL;
static int ret = -EFAULT;


static char saidaDados[65];

static struct crypto_shash *sha256;
static struct shash_desc *shash;





static unsigned int test_skcipher_encdec(struct skcipher_def *sk,int enc)
{
    int rc = 0;

    if (enc)
        rc = crypto_skcipher_encrypt(sk->req);
    else
        rc = crypto_skcipher_decrypt(sk->req);

    switch (rc) {
    case 0:
        break;
    case -EINPROGRESS:
    case -EBUSY:
        rc = wait_for_completion_interruptible(
            &sk->result.completion);
        if (!rc && !sk->result.err) {
            reinit_completion(&sk->result.completion);
            break;
        }
    default:
        pr_info("skcipher encrypt returned with %d result %d\n",
            rc, sk->result.err);
        break;
    }
    init_completion(&sk->result.completion);
    pr_info("skcipher encrypt returned with %d result %d\n",rc, sk->result.err);
    return rc;
}




void printaBinario(int numero){
    printk(KERN_INFO "B = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(numero));
 
}















static void show_hash_result(char * plaintext, char * hash_sha256)
{
    int i;
    char str[32*2 + 1];

    printk("sha256 test for string: \"%s\"\n", plaintext);
    for (i = 0; i < 32 ; i++)
        sprintf(&str[i*2],"%02x", (unsigned char)hash_sha256[i]);
    str[i*2] = 0;
    printk("%s\n", str);
}








void converteHexaParaChave(char chaveEnt[], char chaveFim[], int qtd){
int i;
char aux1, aux2;
for(i=0;i<qtd;i++)
{
 
    aux1 = chaveEnt[i];
    aux2 = chaveEnt[i];
    aux1 = (aux1 >> 4) & 15;
    aux2 = aux2 & 15;
   // printf("Leading text volta0 "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(chaveEnt[i]));
//    printf("\n");
   // printf("Leading text volta1 "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(aux1));
//    printf("\n");
  //   printf("Leading text volta2 "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(aux2));
    //printf("\n\n\n");
 
    if(aux1<10)
    {
        aux1 = aux1 + 48;
    }
    else{
        aux1 = aux1 + 55;
    }
    if(aux2<10)
    {
        aux2 = aux2 + 48;
    }
    else{
        aux2 = aux2 + 55;
    }
    chaveFim[i*2] = aux1;
    chaveFim[i*2 + 1] = aux2;
}
 
}


void converteChaveParaHexa(char aString[], char chaveFim[], int qtd){
int i, cont,j=0;
char aux[32];
    cont = strlen(aString);
    //printk(KERN_INFO "cont eh %i o",cont);
    if(cont != 32){
        cont = cont-1;
        j=0;
        for (i=31;i>-1;i--){
            if(cont<0)
            {
                aux[i] = 0;
            }
            else{
                aux[i] = aString[cont];
                cont--;
            }
         
         
        }
        for (i=0;i<32;i++){
            aString[i] = aux[i];
        }
     
    }
    j=0;
 
    for (i = 0; i < qtd*2; i++)
    {
        if (aString[i] > 47 && aString[i] < 58)
        {
            aString[i] = aString[i] - 48;
        }
        else if (aString[i]>64 && aString[i] <71)
        {
            aString[i] = aString[i] - 55;
        }
 
    }
    for (i = 0; i < qtd*2; i=i+2)
        {
         
            chaveFim[j] = (aString[i] = aString[i] << 4) + aString[i+1];
            //https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
           //printk(KERN_INFO "Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(chaveFim[j]));
            j++;
        }
}


static int __init ebbchar_init(void){
    printk(KERN_INFO "Meu modulo inicializou: minha string eh %s\n", mystring);
/// arrumar criptografia

    skcipher = crypto_alloc_skcipher("ecb(aes)", 0, 0);
        if (IS_ERR(skcipher)) {
        printk(KERN_ALERT "EBBChar não conseguiu criar a cifra\n");
        return PTR_ERR(skcipher);
        }
     printk(KERN_ALERT "EBBChar Criou a cifra\n");

    req = skcipher_request_alloc(skcipher, GFP_KERNEL);
    if (!req) {
        printk(KERN_ALERT "EBBChar não alocou a requisição\n");
       }
    printk(KERN_ALERT "EBBChar Alocou a requisição\n");

    skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,test_skcipher_cb,&sk.result);

    converteChaveParaHexa(mystring, final,16);



    scratchpad = kmalloc(16, GFP_KERNEL);
    if (crypto_skcipher_setkey(skcipher, final, 16)) {
                pr_info("key could not be set\n");
            ret = -EAGAIN;
        if (skcipher)
    {
        crypto_free_skcipher(skcipher);
    } 
        if (req)
    {
        skcipher_request_free(req);
    }
        if (scratchpad)
    {
        kfree(scratchpad);
    }
        if (testeC)
    {
        kfree(testeC);
    }
      
        return ret;
        }
    else{
        pr_info("a could set e tamanho do bloco %i\n", crypto_skcipher_blocksize(skcipher));
    }

 /* Input data will be random */

    //get_random_bytes(scratchpad, 16);

///////////////////
    sk.tfm = skcipher;
    sk.req = req;


    pr_info("Encryption triggered successfully\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "EBBChar failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "EBBChar: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ebbcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ebbcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "EBBChar: device class registered correctly\n");

   // Register the device driver
   ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ebbcharDevice)){               // Clean up if there is an error
      class_destroy(ebbcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ebbcharDevice);
   }
   printk(KERN_INFO "EBBChar: device class created correctly\n"); // Made it! device was initialized
   return 0;
}


static void __exit ebbchar_exit(void){
   device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number


crypto_free_skcipher(skcipher);
skcipher_request_free(req);
kfree(scratchpad);
kfree(testeC);


   printk(KERN_INFO "EBBChar: Goodbye from the LKM!\n");
}


static int dev_open(struct inode *inodep, struct file *filep){
  // numberOpens++;
   //printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);
   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
 
char dado[33];
int m;
char dadoHexa[16];
char scratchpadAux[16];
char hash_sha256[32];
for(m=0;m<(len-2);m++){
dado[m] = buffer[m+2];
//printk(KERN_INFO "EBBChar: dado i eh %c", dado[m]);
}
for(m=m;m<32;m++){
dado[m] = 0;
}
dado[32] = '\0';

printk(KERN_INFO "EBBChar: dado %s", dado);



converteChaveParaHexa(dado,dadoHexa,16);
for(m=0;m<16;m++){
scratchpad[m] = dadoHexa[m];
//printk(KERN_INFO "Leading TEXT DADO "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(dadoHexa[m]));
}


    sg_init_one(&sk.sg, scratchpad, 16);
    skcipher_request_set_crypt(req, &sk.sg, &sk.sg, 16, NULL);
     init_completion(&sk.result.completion);

///////////// criptografia de dados
    if(buffer[0] == 'C'){

     printk(KERN_INFO "EBBChar: Receive CRIPTO e dado %s", dado);
    ret = test_skcipher_encdec(&sk, 0);

for(m=0;m<16;m++){
scratchpadAux[m] = scratchpad[m];
}
    converteHexaParaChave(scratchpadAux, saidaDados,16);
saidaDados[32] = '\0';




}

/////////////// DESCRIPTOGRAFAR DADOS
else if(buffer[0] == 'D'){
    printk(KERN_INFO "EBBChar: Receive DESCRIPTO e dado %s", dado);


ret = test_skcipher_encdec(&sk, 1);
    converteHexaParaChave(scratchpad, saidaDados,16);
saidaDados[32] = '\0';


}
///////////// HASH DE DADOS
else{
    printk(KERN_INFO "EBBChar: Receive HASHTAG e dado %s", dado);


sha256 = crypto_alloc_shash("sha256", 0, 0);
    if (IS_ERR(sha256))
        return -1;
    shash =
        kmalloc(sizeof(struct shash_desc) + crypto_shash_descsize(sha256),
                GFP_KERNEL);
    if (!shash)
        return -ENOMEM;

    shash->tfm = sha256;
    shash->flags = 0;
    if (crypto_shash_init(shash))
        return -1;

    if (crypto_shash_update(shash, scratchpad, 16))
        return -1;

    if (crypto_shash_final(shash, hash_sha256))
        return -1;

    kfree(shash);
    crypto_free_shash(sha256);
    show_hash_result(scratchpad, hash_sha256);
converteHexaParaChave(hash_sha256, saidaDados,32);
saidaDados[64] = '\0';

}

sprintf(message, "%s", saidaDados);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "EBBChar: Received %zu characters from the user e a mensagem %s primeiro caracter eh %c\n", len, buffer, buffer[0]);

   return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "EBBChar: Device successfully closed\n");
   return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(ebbchar_init);
module_exit(ebbchar_exit);
