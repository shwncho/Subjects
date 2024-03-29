public class Account {
    private String name;
    private double balance;


    public String check(){
        return "name: "+name+ " balance: "+balance;
    }

    public synchronized void deposit(double cache){
        balance+=cache;
    }
    public synchronized void withdraw(double cache){
        balance-=cache;
    }
    public synchronized void transfer(Account receiver, double cache){
        receiver.deposit(cache);
        this.balance-=cache;
    }

    public Account(String name, double balance) {
        this.name = name;
        this.balance = balance;
    }

    public String getName() {
        return name;
    }

    public double getBalance() {
        return balance;
    }
}
