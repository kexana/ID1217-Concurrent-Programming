import java.util.Random;

/** monitor object to service maxVehicles number of threads */
public class Station {
    private int maxVehicles;
    private int maxNitroStorage;
    private int maxQuantStorage;
    public int nitroAmount;
    public int quantAmount;
    private int docked;

    private Random rnd;

    /** monitor object to service maxVehicles number of threads */
    public Station(int maxVehicles, int maxNitroStorage, int maxQuantStorage) {
        this.maxVehicles = maxVehicles;
        this.maxNitroStorage = maxNitroStorage;
        this.maxQuantStorage = maxQuantStorage;
        this.nitroAmount = 0;
        this.quantAmount = 0;
        this.docked = 0;
        this.rnd = new Random();
    }

    /** call this function from any vehichle to fuel up the vehicle */
    public synchronized void FuelUp(int nitroRequestAmount, int quantRequestAmount) {

        while (this.docked >= this.maxVehicles) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }

        // succesfully docked
        docked++;
        this.CoreFuleUpFuncrionality(nitroRequestAmount, quantRequestAmount);
        docked--;
        notifyAll();
    }

    private void CoreFuleUpFuncrionality(int nitroRequestAmount, int quantRequestAmount) {
        while ((nitroRequestAmount > 0 && this.nitroAmount < nitroRequestAmount)
                || (quantRequestAmount > 0 && this.quantAmount < quantRequestAmount)) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }

        try {
            Thread.sleep(rnd.nextInt(100)); // stall for believable refuling
        } catch (InterruptedException e) {
            System.out.println(e.getMessage());
        }

        this.nitroAmount -= nitroRequestAmount;
        this.quantAmount -= quantRequestAmount;

        System.out.println("Fuel Up: The space vehicle: " + Thread.currentThread().getName() + " took " + nitroRequestAmount
                + " units nitrogen;" + quantRequestAmount + " units quantum fuel from the station");
    }

    /**
     * call this function from a supply vehichle to supply the station's tanks with
     * fuel and also internaly calls the CoreFuelUpFunctionality function to manage docking counting better
     */
    public synchronized void RechargeFuelTanksAndFuelUp(int nitroSupplyAmount, int quantSupplyAmount,
            int nitroRequestAmount, int quantRequestAmount) {

        while (this.docked >= this.maxVehicles) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }

        // succesfully docked
        docked++;

        while ((nitroSupplyAmount > 0 && this.nitroAmount + nitroSupplyAmount > this.maxNitroStorage)
                || (quantSupplyAmount > 0 && this.quantAmount + quantSupplyAmount > this.maxQuantStorage)) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }

        try {
            Thread.sleep(rnd.nextInt(900)); // stall for believable recharging
        } catch (InterruptedException e) {
            System.out.println(e.getMessage());
        }

        this.nitroAmount += nitroSupplyAmount;
        this.quantAmount += quantSupplyAmount;

        System.out.println("Recharge Station: The supply vehicle: " + Thread.currentThread().getName() + " supplied " + nitroSupplyAmount
                + " units nitrogen;" + quantSupplyAmount + " units quantum fuel to the station");

        //fuel up the supplier vehicle
        this.CoreFuleUpFuncrionality(nitroSupplyAmount, quantSupplyAmount);

        

        docked--;
        notifyAll();
    }

}
