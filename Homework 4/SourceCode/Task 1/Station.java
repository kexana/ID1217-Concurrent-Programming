import java.util.Random;

/** monitor object to service maxVehicles number of threads */
public class Station {
    private int maxVehicles;
    private int maxNitroStorage;
    private int maxQuantStorage;
    public int nitroAmount;
    public int quantAmount;
    private int docked;
    private long generalTimeout;

    private Random rnd;

    /** monitor object to service maxVehicles number of threads */
    public Station(int maxVehicles, int maxNitroStorage, int maxQuantStorage, long generalTimeout) {
        this.maxVehicles = maxVehicles;
        this.maxNitroStorage = maxNitroStorage;
        this.maxQuantStorage = maxQuantStorage;
        this.nitroAmount = 0;
        this.quantAmount = 0;
        this.docked = 0;
        this.rnd = new Random();
        this.generalTimeout = generalTimeout;
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
        System.out.println(Thread.currentThread().getName() + " docked ");
        this.CoreFuleUpFuncrionality(nitroRequestAmount, quantRequestAmount);
        System.out.println(Thread.currentThread().getName() + " flew away");
        docked--;
        notifyAll();
    }

    private void CoreFuleUpFuncrionality(int nitroRequestAmount, int quantRequestAmount) {
        long beginTime = System.currentTimeMillis();
        while ((nitroRequestAmount > 0 && this.nitroAmount < nitroRequestAmount)
                || (quantRequestAmount > 0 && this.quantAmount < quantRequestAmount)) {
            try {
                wait(this.generalTimeout);
                // This timeout measure is simply here for testing purposes since, the fuel and
                // request amounts of each vehicle are random it often happens that they do not
                // coincide and reach a stall. in a proper program these amounts would somehow
                // be properly balanced
                if (System.currentTimeMillis() - beginTime >= this.generalTimeout) {
                    System.out.println(Thread.currentThread().getName() + " - waited too long whithout service");
                    return;
                }
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

        System.out.println(
                "Fuel Up: The space vehicle: " + Thread.currentThread().getName() + " took " + nitroRequestAmount
                        + " units nitrogen;" + quantRequestAmount + " units quantum fuel from the station");
        System.out.println("N: " + this.nitroAmount + " Q: " + this.quantAmount + "\n");

    }

    /**
     * call this function from a supply vehichle to supply the station's tanks with
     * fuel and also internaly calls the CoreFuelUpFunctionality function to manage
     * docking counting better
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
        System.out.println(Thread.currentThread().getName() + " docked ");
        long beginTime = System.currentTimeMillis();
        while ((nitroSupplyAmount > 0 && this.nitroAmount + nitroSupplyAmount > this.maxNitroStorage)
                || (quantSupplyAmount > 0 && this.quantAmount + quantSupplyAmount > this.maxQuantStorage)) {
            try {
                wait(this.generalTimeout);
                // This timeout measure is simply here for testing purposes since, the fuel and
                // request amonts of each vehicle are random it often happens that they do not
                // coincide and reach a stall. in a proper program these amounts would somehow
                // be properly balanced
                if (System.currentTimeMillis() - beginTime >= this.generalTimeout) {
                    System.out.println(
                            Thread.currentThread().getName() + " - waited too long whithout being able to recharge");
                    docked--;
                    notifyAll();
                    return;
                }
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

        System.out.println("Recharge Station: The supply vehicle: " + Thread.currentThread().getName() + " supplied "
                + nitroSupplyAmount
                + " units nitrogen;" + quantSupplyAmount + " units quantum fuel to the station");
        System.out.println("N: " + this.nitroAmount + " Q: " + this.quantAmount + "\n");

        // fuel up the supplier vehicle
        this.CoreFuleUpFuncrionality(nitroRequestAmount, quantRequestAmount);

        System.out.println(Thread.currentThread().getName() + " flew away");
        docked--;
        notifyAll();
    }

}
