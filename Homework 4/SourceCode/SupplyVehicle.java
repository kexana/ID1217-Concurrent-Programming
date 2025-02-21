public class SupplyVehicle extends Vehicle {

    private int nitroSupplyAmount;
    private int quantSupplyAmount;

    public SupplyVehicle(int nitroRequestAmount, int quantRequestAmount, Station fuelStation, int lifeSpan,
            int nitroSupplyAmount, int quantSupplyAmount) {
        super(nitroRequestAmount, quantRequestAmount, fuelStation, lifeSpan);
        this.nitroSupplyAmount = nitroSupplyAmount;
        this.quantSupplyAmount = quantSupplyAmount;
    }

    @Override
    public void run() {

        while (this.lifeSpan > 0) {
            this.fuelStation.RechargeFuelTanksAndFuelUp(this.nitroSupplyAmount, this.quantSupplyAmount,
                    this.nitroRequestAmount, this.quantRequestAmount);
            lifeSpan--;
            try {
                Thread.sleep(rnd.nextInt(5000)); // do stuff before next request
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }
        System.out.println(Thread.currentThread().getName() +" Finished");
    }

}
