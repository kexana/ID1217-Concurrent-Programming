import java.util.Random;

public class FuelSpaceStationMain {

    public static void main(String[] args) {

        // args[0] number of all vehichles
        // args[1] number of suppliers
        // args[2] station nitro capacity
        // args[3] station quant capacity

        int numberOfVehicles = Integer.parseInt(args[0]);
        int numberOfSuppliers = Integer.parseInt(args[1]);
        int stationMaxNitro = Integer.parseInt(args[2]);
        int stationMaxQuant = Integer.parseInt(args[3]);
        Vehicle[] allVehicles = new Vehicle[numberOfVehicles];
        Random rnd = new Random();

        //how many station access cycles each type of vehicle gets for the simulation
        int supplierLifeSpan = 3;
        int standardLifeSpan = 3;

        Station station = new Station(numberOfVehicles/2+1, stationMaxNitro, stationMaxQuant, 5000);

        for (int i = 0; i < numberOfVehicles; i++) {
            if (i < numberOfSuppliers) {
                allVehicles[i] = new SupplyVehicle(rnd.nextInt(stationMaxNitro/5), rnd.nextInt(stationMaxQuant/5), station, supplierLifeSpan,
                        rnd.nextInt(2*(stationMaxNitro / 3)), rnd.nextInt(2*(stationMaxQuant / 3)));
            } else {
                allVehicles[i] = new OrdinaryVehicle(rnd.nextInt(stationMaxNitro/5), rnd.nextInt(stationMaxQuant/5), station, standardLifeSpan);
            }
            allVehicles[i].start();
        }

        for (int i = 0; i < numberOfVehicles; i++) {
            try {
                allVehicles[i].join();
            } catch (InterruptedException e) {
                System.out.println(e.getMessage());
            }
        }
        System.out.println("Finished");
    }
}