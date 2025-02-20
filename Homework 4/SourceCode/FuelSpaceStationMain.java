import java.util.Random;

public class FuelSpaceStationMain {

    public static void main(String[] args) {

        // args[0] number of vehichles
        // args[1] number of suppliers
        // args[2] station nitro capacity
        // args[3] station quant capacity

        int numberOfVehicles = Integer.parseInt(args[0]);
        int numberOfSuppliers = Integer.parseInt(args[1]);
        int stationMaxNitro = Integer.parseInt(args[2]);
        int stationMaxQuant = Integer.parseInt(args[3]);
        Vehicle[] allVehicles = new Vehicle[numberOfVehicles];
        Random rnd = new Random();

        int supplierLifeSpan = 3;
        int standardLifeSpan = 5;

        Station station = new Station(numberOfVehicles, stationMaxNitro, stationMaxQuant);

        for (int i = 0; i < numberOfVehicles; i++) {
            if (i < numberOfSuppliers) {
                allVehicles[i] = new SupplyVehicle(rnd.nextInt(100), rnd.nextInt(100), station, supplierLifeSpan,
                        rnd.nextInt(stationMaxNitro / 2), rnd.nextInt(stationMaxQuant / 2));
            } else {
                allVehicles[i] = new OrdinaryVehicle(rnd.nextInt(100), rnd.nextInt(100), station, standardLifeSpan);
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
    }
}