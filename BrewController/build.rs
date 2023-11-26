// build.rs

fn main() {
    // Link the native library libGenericDrivers.so
    println!("cargo:rustc-link-lib=GenericDrivers");
    
    // Set the path to the directory containing libGenericDrivers.so
    println!("cargo:rustc-link-search=native=.");
}
