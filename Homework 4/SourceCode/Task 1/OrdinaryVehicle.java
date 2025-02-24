public class OrdinaryVehicle extends Vehicle {

    public OrdinaryVehicle(int nitroRequestAmount, int quantRequestAmount, Station fuelStation, int lifeSpan) {
        super(nitroRequestAmount, quantRequestAmount, fuelStation, lifeSpan);
    }
    
    
    @Override
    public void run() {
        this.StandardFuelUpProcedure(2000);
        System.out.println(Thread.currentThread().getName() +" Finished");
    }

}
