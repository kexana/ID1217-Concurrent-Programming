import java.util.Random;

public abstract class Vehicle extends Thread {
    protected int nitroRequestAmount;
    protected int quantRequestAmount;
    protected Station fuelStation;

    protected int lifeSpan; //how many request cycles can the vehicles do

    protected Random rnd;

    public Vehicle(int nitroRequestAmount, int quantRequestAmount, Station fuelStation, int lifeSpan) {
        this.nitroRequestAmount = nitroRequestAmount;
        this.quantRequestAmount = quantRequestAmount;
        this.fuelStation = fuelStation;
        this.rnd = new Random();
        this.lifeSpan = lifeSpan;
    }

    /**
     * @param delay the amount of time the vehicle spends away from the station
     * might be posible to rework this function to also work for the supplier somehow by passing a line of code as an argument?
    */
    public void StandardFuelUpProcedure(int delay) {
        while(this.lifeSpan > 0){
            this.fuelStation.FuelUp(this.nitroRequestAmount, this.quantRequestAmount);
            this.lifeSpan--;
            try {
                Thread.sleep(rnd.nextInt(delay)); //do stuff before next request
            } 
            catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }
    }
}
