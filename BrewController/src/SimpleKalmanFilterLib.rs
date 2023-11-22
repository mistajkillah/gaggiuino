pub struct SimpleKalmanFilter {
    err_measure: f32,
    err_estimate: f32,
    q: f32,
    current_estimate: f32,
    last_estimate: f32,
    kalman_gain: f32,
}

impl SimpleKalmanFilter {
    pub fn new(mea_e: f32, est_e: f32, q: f32) -> Self {
        Self {
            err_measure: mea_e,
            err_estimate: est_e,
            q,
            current_estimate: 0.0,
            last_estimate: 0.0,
            kalman_gain: 0.0,
        }
    }

    pub fn update_estimate(&mut self, mea: f32) -> f32 {
        self.kalman_gain = self.err_estimate / (self.err_estimate + self.err_measure);
        self.current_estimate =
            self.last_estimate + self.kalman_gain * (mea - self.last_estimate);
        self.err_estimate =
            (1.0 - self.kalman_gain) * self.err_estimate + (self.last_estimate - self.current_estimate).abs() * self.q;
        self.last_estimate = self.current_estimate;

        self.current_estimate
    }

    pub fn set_measurement_error(&mut self, mea_e: f32) {
        self.err_measure = mea_e;
    }

    pub fn set_estimate_error(&mut self, est_e: f32) {
        self.err_estimate = est_e;
    }

    pub fn set_process_noise(&mut self, q: f32) {
        self.q = q;
    }

    pub fn get_kalman_gain(&self) -> f32 {
        self.kalman_gain
    }

    pub fn get_estimate_error(&self) -> f32 {
        self.err_estimate
    }
}
